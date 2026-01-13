#include "World.h"
#include "Game.h"
#include "Player.h"
#include "Camera.h"
#include "Random.h"
#include "amath.h"
#include <stack>
#include <set>

World::World()
{
}

World::~World()
{
	if (m_isRunning)
	{
		m_isRunning = false;
		for (size_t i = 0; i < m_workerThreads.size(); i++)
		{
			if (m_workerThreads[i].joinable())
			{
				m_workerThreads[i].join();
			}
		}
	}
}

void World::Init(Game* game)
{
	m_game = game;
	m_atlas = game->m_atlas.get();
	m_lightManager.Init(this);

	m_isRunning = true;
	int workerThreadsCount{ 6 };
	if (m_game->IsLinux())
	{
		workerThreadsCount = 8;
	}
	else
	{
		workerThreadsCount = 20;
	}
	for (int i = 0; i < workerThreadsCount; i++)
	{
		m_workerThreads.push_back(std::thread(&World::AsyncUpdate, this));
	}
}

void World::OnQuit()
{
	{
		std::unique_lock<std::mutex> lock(m_chunksMutex);
		for (std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
		{
			const Chunk* const chunk{ it->second.get() };
			if (chunk->ShouldSave())
			{
				m_game->m_saver.SaveChunk(chunk);
			}
		}
	}
}

void World::SetGenerator(std::unique_ptr<WorldGen> worldgen)
{
	m_generator = std::move(worldgen);
}

WorldGen* World::GetGenerator() const
{
	return m_generator.get();
}

bool World::ShouldGenerate(glm::ivec3 chunkPos, glm::ivec3 playerChunkPos)
{
	const float distance{ glm::length(glm::vec3(chunkPos.x - playerChunkPos.x, 0, chunkPos.z - playerChunkPos.z)) };

	const bool isInRange{
		distance < static_cast<float>(m_generationDistance)
	};
	bool isInFrustum{ m_game->m_player->m_camera->IsInFrustum(glm::vec3(chunkPos.x * Chunk::XWIDTH, chunkPos.y * Chunk::YHEIGHT, chunkPos.z * Chunk::ZDEPTH), false) };
	return (isInRange && isInFrustum) || distance < 2.0f;
}

void World::Update()
{
	m_generationDistance = m_game->GetGamerules().m_generationDistance;
	const glm::ivec3 chunkPos{ World::WorldBlockToChunkGrid(m_game->m_player->m_transform.GetWorldPosition()) };
	{
		std::lock_guard<std::mutex> mapLock(m_chunksMutex);
		int x = 0, z = 0;
		int dx = 0, dz = -1;

		int maxChunks = (2 * m_generationDistance + 1) * (2 * m_generationDistance + 1);

		for (int i = 0; i < maxChunks; i++)
		{
			const glm::ivec3 check(chunkPos.x + x, 0, chunkPos.z + z);

			if (ShouldGenerate(check, chunkPos))
			{
				Chunk* chunk{ GetChunkAt(check) };
				if (chunk == nullptr)
				{
					GenerateChunkAt(check);
					chunk = GetChunkAt(check);
					InitGenerate(chunk);
				}
				if (chunk->m_isDirty && !chunk->m_isGenerating)
				{
					GenerateMesh(chunk);
				}
			}

			if (x == z || (x < 0 && x == -z) || (x > 0 && x == 1 - z))
			{
				int temp = dx;
				dx = -dz;
				dz = temp;
			}
			x += dx;
			z += dz;
		}
		m_lightManager.EndFrame();
	}

	Chunk* chunk;
	if (m_toUpdateGPUBuffersQueue.tryPop(chunk))
	{
		chunk->UpdateGPUBuffers();
	}
}

RemeshRequest World::MakeRequest(Chunk* chunk)
{
	Chunk* North = GetChunkAt(chunk->m_position + glm::ivec3(0, 0, 1));
	Chunk* South = GetChunkAt(chunk->m_position + glm::ivec3(0, 0, -1));
	Chunk* Up = GetChunkAt(chunk->m_position + glm::ivec3(0, 1, 0));
	Chunk* Down = GetChunkAt(chunk->m_position + glm::ivec3(0, -1, 0));
	Chunk* East = GetChunkAt(chunk->m_position + glm::ivec3(1, 0, 0));
	Chunk* West = GetChunkAt(chunk->m_position + glm::ivec3(-1, 0, 0));
	return RemeshRequest(chunk, North, South, Up, Down, East, West);
}

void World::InitGenerate(Chunk* chunk)
{
	chunk->m_isDirty = false;

	m_toGenerateQueue.pushBack(MakeRequest(chunk));
}

void World::GenerateMesh(Chunk* chunk)
{
	chunk->m_isDirty = false;

	m_toMeshQueue.pushBack(MakeRequest(chunk));
}

void World::GenerateChunkGrid(RemeshRequest& request)
{
	assert(request.chunk);
	
	if (request.chunk->m_isSaving) return;

	request.chunk->m_isGenerating = true;
	if (!m_game->m_saver.LoadChunk(request.chunk))
	{
		m_generator->GenerateChunkGrid(request);
		m_lightManager.GenerateSkyExposure(request);
	}
	request.chunk->m_isGenerating = false;
	request.chunk->m_isReadable = true;
}

RaycastResult World::Raycast(Ray ray)
{
	static constexpr float backstep{ 0.04f };
	static constexpr float step{ 0.075f };
	RaycastResult hit{ SimpleRaycast(ray, step, false) };
	RaycastResult ret{ SimpleRaycast(Ray(hit.m_worldPos, hit.m_worldPos - ray.Direction() * 2.0f), backstep, true)};
	glm::ivec3 normal{ ret.m_worldBlockPos - hit.m_worldBlockPos };
	RaycastResult result(hit.m_block, hit.m_worldBlockPos, ret.m_worldPos, hit.m_chunk, normal);

	return result;
}

RaycastResult World::SimpleRaycast(Ray ray, float step, bool invert)
{
	const int iterations{ static_cast<int>(ray.Length() / step) };
	const glm::vec3 direction{ ray.Direction() };
	glm::vec3 check{ ray.m_start };
	for (int i = 0; i < iterations; i++)
	{
		check += direction * step;
		glm::ivec3 gridCheck(floorf(check.x), floorf(check.y), floorf(check.z));
		glm::ivec3 chunkPos{ World::WorldBlockToChunkGrid(gridCheck) };
		Chunk* const chunk{ GetChunkAt(chunkPos) };
		if (!chunk) continue;
		std::lock_guard<std::mutex> lock(chunk->m_mtx);

		glm::ivec3 local{ chunk->WorldToLocal(gridCheck) };
		Block* const block{ chunk->AtSafe(local) };
		const bool isSolidHit{ block && BlockData::IsSolid(block->GetID()) && !invert };
		const bool isOtherHit{ !block || (!BlockData::IsSolid(block->GetID()) && invert) };
		if (isSolidHit || isOtherHit)
		{
			RaycastResult result(block, gridCheck, check, chunk, glm::ivec3(0, 0, 0));
			return result;
		}
	}
	return RaycastResult(nullptr, glm::ivec3(0, 0, 0), glm::vec3(0, 0, 0), nullptr, glm::ivec3(0, 0, 0));
}

std::vector<Chunk*> World::GetNeighbours(Chunk* chunk, bool includeSelf, bool includeCorners)
{
	std::vector<Chunk*> result;
	
	const glm::ivec3* shifts{ nullptr };
	int len{ 0 };

	if (includeCorners)
	{
		shifts = allXZNeighbours;
		len = 8;
	}
	else
	{
		shifts = closeXZNeighbours;
		len = 4;
	}

	if (includeSelf)
	{
		result.push_back(chunk);
	}
	
	for (int i = 0; i < len; i++)
	{
		const glm::ivec3& shift = shifts[i];
		Chunk* n = GetChunkAt(chunk->m_position + shift);
		if (n && n->m_isReadable) result.push_back(n);
	}
	return result;
}

void World::FixedUpdate()
{
	std::lock_guard<std::mutex> mapLock(m_chunksMutex);
	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>>::iterator it;
	const glm::ivec3 playerChunk{ World::WorldBlockToChunkGrid(m_game->m_player->m_transform.GetLocalPosition()) };
	for (it = m_chunks.begin(); it != m_chunks.end(); /*custom*/)
	{
		const glm::ivec3 gridpos{ it->first };
		Chunk* const chunk{ it->second.get() };
		const float distance{ glm::length(glm::vec3(gridpos) - glm::vec3(playerChunk)) };
		if ((distance > static_cast<float>(m_generationDistance) * DELETION_DISTANCE_FACTOR && chunk->m_isGenerating == false || chunk->m_isDiscarded) && chunk->m_controls == 0 && !chunk->m_isSaving)
		{
			chunk->m_isReadyForRender = false;
			if (chunk->m_isCustom)
			{
				chunk->m_isSaving = true;
				m_game->m_saver.SaveChunk(chunk);
				chunk->m_isSaving = false;
			}
			it = m_chunks.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void World::AsyncUpdate()
{
	while (m_isRunning)
	{
		RemeshRequest request;
		if (m_toGenerateQueue.tryPop(request))
		{
			if (!ShouldGenerate(request.chunk->m_position, WorldBlockToChunkGrid(m_game->m_player->m_transform.GetWorldPosition())))
			{
				request.chunk->m_isDiscarded = true;
				request.chunk->m_isGenerating = false;
				request.ResetControl();
			}
			else
			{
				GenerateChunk(request);
			}
		}
		if (m_toMeshQueue.tryPop(request))
		{
			if (!ShouldGenerate(request.chunk->m_position, WorldBlockToChunkGrid(m_game->m_player->m_transform.GetWorldPosition())))
			{
				request.chunk->m_isDiscarded = true;
				request.chunk->m_isGenerating = false;
				request.ResetControl();
			}
			else
			{
				MeshChunk(request);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void World::GenerateChunk(RemeshRequest& request)
{
	Chunk* chunk{ request.chunk };
	chunk->m_isGenerating = true;
	{
		std::lock_guard<std::mutex> lock(chunk->m_mtx);
		if (chunk->m_version == 0)
		{
			GenerateChunkGrid(request);
			chunk->m_version++;
			chunk->m_isDirty = true;
		}
	}
	request.ResetControl();
}

void World::MeshChunk(RemeshRequest& request)
{
	Chunk* chunk{ request.chunk };

	if (request.chunk->m_isSaving)
	{
		request.ResetControl();
		return;
	}

	chunk->m_isGenerating = true;
	bool pushToGpu{ false };
	{
		std::lock_guard<std::mutex> lock(chunk->m_mtx);
		bool allNeighboursGenerated{ false };
		allNeighboursGenerated = request.AreAllGeneratedXZ();
		if (chunk->m_version > 0 && allNeighboursGenerated)
		{
			chunk->m_version++;
			chunk->m_mesh.ClearGeometry();
			chunk->m_waterMesh.ClearGeometry();
			pushToGpu = true;
			for (int x = 0; x < Chunk::XWIDTH; x++)
			{
				for (int z = 0; z < Chunk::ZDEPTH; z++)
				{
					for (int y = 0; y < Chunk::YHEIGHT; y++)
					{
						glm::ivec3 locPos(x, y, z);
						Block* block = chunk->AtSafe(locPos);
						if (!block || BlockData::IsAir(block->GetID())) continue;

						chunk->GenerateBlock(locPos, *block, &request);
					}
				}
			}
		}
		request.ResetControl();
		if (!pushToGpu)
		{
			chunk->m_isGenerating = false;
			chunk->m_isDirty = true;
		}
	}
	if (pushToGpu)
	{
		m_toUpdateGPUBuffersQueue.pushBack(chunk);
	}
	else
	{
		chunk->m_isGenerating = false;
	}
}

Chunk* World::GetChunkAt(glm::ivec3 pos)
{
	if (m_chunks.find(pos) == m_chunks.end()) return nullptr;
	return m_chunks.at(pos).get();
}

void World::GenerateChunkAt(glm::ivec3 chunkPos)
{
	//assert(glm::length(glm::vec3(chunkPos)) > 100);
	m_chunks.try_emplace(chunkPos, std::make_unique<Chunk>(chunkPos, m_game));
}

void World::Render(Player* player)
{
	m_atlas->Use();
	{
		std::lock_guard<std::mutex> mapLock(m_chunksMutex);
		m_chunksRendered = 0;
		m_chunksActive = 0;
		std::vector<Chunk*> toRender;
		for (std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
		{
			Chunk* chunk = it->second.get();
			if (!chunk) continue;

			m_chunksActive++;
			
			if (chunk->m_isReadyForRender)
			{
				glm::vec3 pos{ World::ChunkGridToWorldBlock(chunk->m_position) };
				std::array<glm::ivec3, 8> corners = Chunk::GetCorners(chunk->m_position);
				bool shouldRender{ true };
				if (m_game->GetGamerules().m_enableFrustumCulling)
				{
					bool isInFrustum{ false };
					for (int i = 0; i < 8; i++)
					{
						glm::ivec3 ipos = corners[i];
						glm::vec3 fpos = glm::vec3(static_cast<float>(ipos.x), static_cast<float>(ipos.y), static_cast<float>(ipos.z));
						fpos.y = player->m_transform.GetWorldPosition().y;
						isInFrustum |= player->m_camera->IsInFrustum(fpos, false, -player->m_transform.GetLocalForward() * 30.0f);
						if (isInFrustum) break;
					}
					shouldRender = isInFrustum;
				}

				if (shouldRender)
				{
					toRender.push_back(chunk);
					m_chunksRendered++;
				}
			}
			if (BaseDebug::show && chunk->m_isReadable)
			{
				chunk->RenderDebug(player->m_camera.get());
			}
		}

		for (std::vector<Chunk*>::iterator it = toRender.begin(); it != toRender.end(); ++it)
		{
			Chunk* chunk{ *it };
			chunk->RenderSolid(player->m_camera.get());
		}

		if (m_game->GetGamerules().m_renderWater)
		{
			for (std::vector<Chunk*>::iterator it = toRender.begin(); it != toRender.end(); ++it)
			{
				Chunk* chunk{ *it };
				if (chunk->m_waterMesh.IsEmpty()) continue;

				chunk->RenderTransparent(player->m_camera.get());
			}
		}
		/*if (BaseDebug::show)
		{
			for (std::vector<Chunk*>::iterator it = toRender.begin(); it != toRender.end(); ++it)
			{
				Chunk* chunk{ *it };
				chunk->RenderDebug(player->m_camera.get());
			}
		}*/
	}
}

bool World::ForceSetBlockAtWorld(glm::ivec3 blockpos, Block::ID id)
{
	std::lock_guard<std::mutex> lock(m_chunksMutex);
	glm::ivec3 chunkPos = World::WorldBlockToChunkGrid(blockpos);
	Chunk* chunk = GetChunkAt(chunkPos);
	if (!chunk)
	{
		return false;
	}
	glm::ivec3 local = chunk->WorldToLocal(blockpos);
	chunk->NewBlock(local, id);
	return true;
}

bool World::SetAndUpdateBlockAtWorld(glm::ivec3 blockpos, Block::ID id)
{
	glm::ivec3 chunkPos = World::WorldBlockToChunkGrid(blockpos);
	Chunk* chunk = GetChunkAt(chunkPos);
	if (!chunk)
	{
		return false;
	}
	glm::ivec3 local = chunk->WorldToLocal(blockpos);
	chunk->NewBlock(local, id, true);
	std::lock_guard<std::mutex> lock(m_chunksMutex);
	m_game->m_world->GenerateMesh(chunk);
	if (World::IsLocalBlockOnChunkEdge(local))
	{
		std::vector<glm::ivec3> neighbourDiffs = chunk->GetChunkNeighboursAt(local);
		for (glm::ivec3& diff : neighbourDiffs)
		{
			Chunk* neighbour = m_game->m_world->GetChunkAt(chunk->m_position + diff);
			if (neighbour)
			{
				m_game->m_world->GenerateMesh(neighbour);
			}
		}
	}
	return true;
}

Block* World::GetBlockAtWorld(glm::ivec3 pos)
{
	glm::ivec3 chunkPos = World::WorldBlockToChunkGrid(pos);
	Chunk* chunk = GetChunkAt(chunkPos);
	if (!chunk)
	{
		return nullptr;
	}
	glm::ivec3 local = chunk->WorldToLocal(pos);
	return chunk->AtSafe(local);
}
