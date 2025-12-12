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
	m_isRunning = true;
	constexpr int workerThreadsCount{ 20 };
	for (int i = 0; i < workerThreadsCount; i++)
	{
		m_workerThreads.push_back(std::thread(&World::AsyncUpdate, this));
	}
}

World::~World()
{
	m_isRunning = false;
	for (int i = 0; i < m_workerThreads.size(); i++)
	{
		if (m_workerThreads[i].joinable())
		{
			m_workerThreads[i].join();
		}
	}
}

void World::Init(Game* game)
{
	m_game = game;
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
		distance < GENERATION_DISTANCE
	};
	bool isInFrustum{ m_game->m_player->m_camera->IsInFrustum(glm::vec3(chunkPos.x * Chunk::XWIDTH, chunkPos.y * Chunk::YHEIGHT, chunkPos.z * Chunk::ZDEPTH), false) };
	return isInRange && isInFrustum || distance < 2;
}

void World::Update()
{
	const glm::ivec3 chunkPos{ World::WorldBlockToChunkGrid(m_game->m_player->m_transform.GetWorldPosition()) };
	{
		std::lock_guard<std::mutex> mapLock(m_chunksMutex);
		int x = 0, z = 0;
		int dx = 0, dz = -1;

		int maxChunks = (2 * GENERATION_DISTANCE + 1) * (2 * GENERATION_DISTANCE + 1);

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
				}
				if (chunk->m_isDirty)
				{
					Regenerate(chunk);
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
	}
	Chunk* chunk;
	if (m_toUpdateGPUBuffersQueue.tryPop(chunk))
	{
		chunk->UpdateGPUBuffers();
	}
}

void World::Regenerate(Chunk* chunk)
{
	chunk->m_isDirty = false;
	chunk->m_isGenerating = true;

	Chunk* North = GetChunkAt(chunk->m_position + glm::ivec3(0, 0, 1));
	Chunk* South = GetChunkAt(chunk->m_position + glm::ivec3(0, 0, -1));
	Chunk* Up	 = GetChunkAt(chunk->m_position + glm::ivec3(0, -1, 0));
	Chunk* Down	 = GetChunkAt(chunk->m_position + glm::ivec3(0, 1, 0));
	Chunk* East	 = GetChunkAt(chunk->m_position + glm::ivec3(1, 0, 0));
	Chunk* West	 = GetChunkAt(chunk->m_position + glm::ivec3(-1, 0, 0));

	m_toMeshQueue.push(RemeshRequest(chunk, North, South, Up, Down, East, West));
}

void World::GenerateChunkGrid(RemeshRequest& request)
{
	m_generator->GenerateChunkGrid(request);
}

void World::UpdateLightBlock(glm::ivec3 worldpos, char intensity)
{
	static constexpr glm::ivec3 neighbours[6]{
			glm::ivec3(0, 0, 1),
			glm::ivec3(0, 1, 0),
			glm::ivec3(1, 0, 0),
			glm::ivec3(0, 0, -1),
			glm::ivec3(0, -1, 0),
			glm::ivec3(-1, 0, 0),
	}; // TODO: make global

	static std::vector<std::pair<glm::ivec3, char>> queue;
	queue.clear();
	static constexpr int capacity = 4000;
	queue.reserve(capacity);
	size_t head = 0;

	Block* source = GetBlockAtWorld(worldpos);
	if (!source) return;

	source->SetLightLevel(intensity);

	queue.push_back({ worldpos, intensity });
	glm::ivec3 origChunkGridPos = WorldBlockToChunkGrid(worldpos);
	Chunk* origChunk = GetChunkAt(origChunkGridPos);
	Chunk* localChunk = nullptr;

	std::set<Chunk*> dirtyChunks;
	dirtyChunks.insert(origChunk);

	Block::ID blockid = Block::ID::Stone;
	BlockData* blockdata = BlocksDatabase::Get(blockid);
	Block* b = nullptr;
	glm::ivec3 nextpos;
	glm::ivec3 nextChunkGridPos;
	glm::ivec3 local;

	int d = 0;
	int iters = 0;
	while (head < queue.size())
	{
		auto [pos, currentLight] = queue[head++];
		iters++;

		if (currentLight == 1) continue;

		for (d = 0; d < 6; d++)
		{
			glm::ivec3 shift = neighbours[d];
			nextpos = pos + shift;
			nextChunkGridPos = WorldBlockToChunkGrid(nextpos);
			local = WorldToLocalAny(nextpos);
			if (nextChunkGridPos == origChunkGridPos)
			{
				b = origChunk->AtForce(local);
			}
			else
			{
				if (!localChunk || localChunk->m_position != nextChunkGridPos)
				{
					localChunk = GetChunkAt(nextChunkGridPos);
					if (!localChunk) continue;
					dirtyChunks.insert(localChunk);
				}
				b = localChunk->AtForce(local);
			}
			if (!b) continue;

			if (blockid != b->GetID())
			{
				blockid = b->GetID();
				blockdata = b->GetData();
			}
			char targetLight = currentLight - 1;
			if (!blockdata->IsSolid() && b->GetLightLevel() < targetLight)
			{
				b->SetLightLevel(currentLight - 1);
				queue.push_back(std::make_pair(nextpos, targetLight));
			}
		}
	}

	for (auto& dirtyChunk : dirtyChunks)
	{
		dirtyChunk->m_isDirty = true;
	}
}

RaycastResult World::Raycast(Ray ray)
{
	static constexpr float backstep{ 0.085f };
	static constexpr float step{ 0.15f };
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
		if ( (block && BlockData::IsSolid(block->GetID()) && !invert) || 
			(!block || !BlockData::IsSolid(block->GetID()) && invert))
		{
			RaycastResult result(block, gridCheck, check, chunk, glm::ivec3(0, 0, 0));
			return result;
		}
	}
	return RaycastResult(nullptr, glm::ivec3(0, 0, 0), glm::vec3(0, 0, 0), nullptr, glm::ivec3(0, 0, 0));
}

std::vector<Chunk*> World::GetNeighbours(Chunk* chunk)
{
	std::vector<Chunk*> result;
	Chunk* n = GetChunkAt(chunk->m_position + glm::ivec3(0, 0, 1));
	if (n && n->m_isReadable) result.push_back(n);
	n = GetChunkAt(chunk->m_position + glm::ivec3(0, 0, -1));
	if (n && n->m_isReadable) result.push_back(n);
	n = GetChunkAt(chunk->m_position + glm::ivec3(0, 1, 0));
	if (n && n->m_isReadable) result.push_back(n);
	n = GetChunkAt(chunk->m_position + glm::ivec3(0, -1, 0));
	if (n && n->m_isReadable) result.push_back(n);
	n = GetChunkAt(chunk->m_position + glm::ivec3(1, 0, 0));
	if (n && n->m_isReadable) result.push_back(n);
	n = GetChunkAt(chunk->m_position + glm::ivec3(-1, 0, 0));
	if (n && n->m_isReadable) result.push_back(n);
	return result;
}

void World::FixedUpdate()
{
	std::lock_guard<std::mutex> mapLock(m_chunksMutex);
	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>>::iterator it;
	glm::ivec3 playerChunk = World::WorldBlockToChunkGrid(m_game->m_player->m_transform.GetWorldPosition());
	for (it = m_chunks.begin(); it != m_chunks.end();)
	{
		glm::ivec3 gridpos = it->first;
		Chunk* chunk = it->second.get();
		const float distance{ glm::length(glm::vec3(gridpos) - glm::vec3(playerChunk)) };
		if ((distance > GENERATION_DISTANCE * 1.25f && chunk->m_isReadyForRender && chunk->m_isGenerating == false || chunk->m_isDiscarded) && chunk->m_controls == 0)
		{
			chunk->m_isReadyForRender = false;
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
		if (m_toMeshQueue.tryPop(request))
		{
			MeshChunk(request);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void World::MeshChunk(RemeshRequest& request)
{
	Chunk* chunk = request.chunk;
	if (!ShouldGenerate(chunk->m_position, WorldBlockToChunkGrid(m_game->m_player->m_transform.GetWorldPosition())))
	{
		chunk->m_isDiscarded = true;
		request.ResetControl();
		return;
	}

	{
		std::lock_guard<std::mutex> lock(chunk->m_mtx);
		chunk->m_version++;
		if (chunk->m_version == 1)
		{
			GenerateChunkGrid(request);
		}
		else
		{
			chunk->m_mesh.ClearGeometry();
		}
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
	m_toUpdateGPUBuffersQueue.push(chunk);
}

Chunk* World::GetChunkAt(glm::ivec3 pos)
{
	if (m_chunks.find(pos) == m_chunks.end()) return nullptr;
	return m_chunks.at(pos).get();
}

void World::GenerateChunkAt(glm::ivec3 chunkPos)
{
	m_chunks.try_emplace(chunkPos, std::make_unique<Chunk>(chunkPos, m_game));
}

void World::Render(Player* player)
{
	{
		std::lock_guard<std::mutex> mapLock(m_chunksMutex);
		std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>>::iterator it;
		CHUNKS_RENDERED = 0;
		CHUNKS_ACTIVE = 0;
		for (it = m_chunks.begin(); it != m_chunks.end(); ++it)
		{
			if (!it->second) continue;

			CHUNKS_ACTIVE++;
			
			if (it->second && it->second->m_isReadyForRender)
			{
				glm::vec3 pos{ World::ChunkGridToWorldBlock(it->second->m_position) };
				std::array<glm::ivec3, 8> corners = Chunk::GetCorners(it->second->m_position);
				bool isInFrustum{ false };
				for (int i = 0; i < 8; i++)
				{
					glm::ivec3 c = corners[i];
					c.y = player->m_transform.GetWorldPosition().y;
					isInFrustum |= player->m_camera->IsInFrustum(c, false);
				}
				if (isInFrustum)
				{
					it->second->Render(player->m_camera.get());
					it->second->RenderDebug(player->m_camera.get());
					CHUNKS_RENDERED++;
				}
			}
		}
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
	chunk->NewBlock(local, id);
	std::lock_guard<std::mutex> lock(m_chunksMutex);
	m_game->m_world->Regenerate(chunk);
	if (World::IsLocalBlockOnChunkEdge(local))
	{
		std::vector<glm::ivec3> neighbourDiffs = chunk->GetChunkNeighboursAt(local);
		for (glm::ivec3& diff : neighbourDiffs)
		{
			Chunk* neighbour = m_game->m_world->GetChunkAt(chunk->m_position + diff);
			if (neighbour)
			{
				m_game->m_world->Regenerate(neighbour);
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
