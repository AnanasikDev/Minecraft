#include "World.h"
#include "Game.h"
#include "Player.h"
#include "Camera.h"
#include "Random.h"

World::World()
{
	m_isRunning = true;
	constexpr int workerThreadsCount = 1;
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

void World::Update()
{
	const glm::ivec3 chunkPos{ m_game->m_player->m_transform.GetChunkPosition() };
	{
		std::lock_guard<std::mutex> mapLock(m_chunksMutex);
		for (int dx = -GENERATION_DISTANCE; dx <= GENERATION_DISTANCE; dx++)
		{
			for (int dz = -GENERATION_DISTANCE; dz <= GENERATION_DISTANCE; dz++)
			{
				const glm::ivec3 check(chunkPos.x + dx, 0, chunkPos.z + dz);
				const float distance{ glm::length(glm::vec3(dx, 0, dz)) };
				
				const bool isInRange{
					distance < GENERATION_DISTANCE
				};
				if (!isInRange) continue;

				/*const glm::vec3 eye{ m_game->m_player->m_transform.GetForward() };
				const glm::vec3 eyehorz(eye.x, 0, eye.z);
				const bool isInFrustum{
					(glm::abs(glm::dot(eyehorz, glm::normalize(glm::vec3(dx, 0, dz))) - 1) < 0.5f) || distance < 1.5f
				};
				if (!isInFrustum) continue;*/
	
				Chunk* chunk{ GetChunkAt(check) };
				if (chunk == nullptr)
				{
					GenerateChunkAt(check);
					chunk = GetChunkAt(check);
				}

				// works no queue
				/*if (chunk->m_isDirty)
				{
					chunk->m_isDirty = false;
					chunk->m_isGenerating = true;
					MeshChunk(chunk);
					chunk->m_isGPUDirty = true;
					chunk->UpdateGPUBuffers();
				}*/

				//if (chunk->m_isGenerating) continue;

				if (chunk->m_isDirty)
				{
					chunk->m_isDirty = false;
					chunk->m_isReadyForRender = false;
					chunk->m_isGPUDirty = false;
					chunk->m_isGenerating = true;
					m_toMeshQueue.push(chunk);
					//printf("Pushed to queue at %d %d (%d)\n", check.x, check.z, chunk->m_version);
				}
			}
		}
	}
	Chunk* chunk;
	while (m_toUpdateGPUBuffersQueue.try_pop(chunk))
	{
		chunk->UpdateGPUBuffers();
		//printf("Updating GPU at %d %d (%d)\n", chunk->m_position.x, chunk->m_position.z, chunk->m_version);
	}
}

void World::FixedUpdate()
{
	/*std::lock_guard<std::mutex> mapLock(m_chunksMutex);
	std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>>::iterator it;
	glm::ivec2 playerChunk = m_game->m_player->m_transform.GetChunkPosition();
	for (it = m_chunks.begin(); it != m_chunks.end();)
	{
		const float distance = glm::length(glm::vec2(it->first) - glm::vec2(playerChunk));
		if (distance > GENERATION_DISTANCE * 1.5f && it->second->m_isReadyForRender && !it->second->m_isGPUDirty && it->second->m_workersAccess == 0)
		{
			it->second->m_isReadyForRender = false;
			it->second->m_workersAccess = -1;
			it = m_chunks.erase(it);
		}
		else
		{
			++it;
		}
	}*/

	/*Chunk* chunk;
	while (m_toMeshQueue.try_pop(chunk))
	{
		MeshChunk(chunk);
	}*/
	
}

void World::AsyncUpdate()
{
	while (m_isRunning)
	{
		Chunk* chunk;
		if (m_toMeshQueue.try_pop(chunk))
		{
			MeshChunk(chunk);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void World::MeshChunk(Chunk* chunk)
{
	{
		std::lock_guard<std::mutex> lock(chunk->m_mtx);
		//printf("Mesing at %d %d (%d)\n", chunk->m_position.x, chunk->m_position.z, chunk->m_version);
		chunk->m_version++;
		chunk->m_mesh.ClearGeometry();
		for (int x = 0; x < Chunk::XWIDTH; x++)
		{
			for (int z = 0; z < Chunk::ZDEPTH; z++)
			{
				for (int y = 0; y < Chunk::YHEIGHT; y++)
				{
					glm::ivec3 locPos(x, y, z);
					Block* block = chunk->At(locPos);
					if (!block || block->IsAir()) continue;

					chunk->GenerateBlock(locPos, *block);
				}
			}
		}
	}
	chunk->m_isGPUDirty = true;
	chunk->m_isGenerating = false;
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

int World::GetHeightAt(glm::ivec3 pos)
{
	//float mountains = fabsf(powf(m_perlin.noise2D(pos.x * MOUNTAINS_NOISE_SCALE, pos.z * MOUNTAINS_NOISE_SCALE), 3.5f)) * 0.0f;
	//return 0.45f + m_perlin.noise2D(pos.x * BASE_NOISE_SCALE, pos.z * BASE_NOISE_SCALE) / 7.0f;// +Random::GetFloat(-0.0075f, 0.0075f);// +mountains;
	//return fabsf(powf(m_perlin.noise2D(x * MOUNTAINS_NOISE_SCALE, z * MOUNTAINS_NOISE_SCALE), 3.5f) * 4.0f);
	return sinf(static_cast<float>(pos.x) / 10.0f) * 8.0f + 10.0f;
}

float World::GetDensityAt(glm::ivec3 pos)
{
	return m_perlin.noise3D
	(
		static_cast<double>(pos.x) * static_cast<double>(CAVES_NOISE_SCALE), 
		static_cast<double>(pos.y) * static_cast<double>(CAVES_NOISE_SCALE), 
		static_cast<double>(pos.z) * static_cast<double>(CAVES_NOISE_SCALE)
	);
}

Block::ID World::GetBlockIDAt(glm::ivec3 worldPos)
{
	const int height{ m_game->m_world->GetHeightAt(worldPos) };
	const float density{ m_game->m_world->GetDensityAt(worldPos) };

	if (worldPos.y < height)
	{
		return Block::ID::Grass;
	}
	return Block::ID::Air;
}

void World::Render(Player* player)
{
	{
		std::lock_guard<std::mutex> mapLock(m_chunksMutex);
		std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>>::iterator it;
		int num = 0;
		for (it = m_chunks.begin(); it != m_chunks.end(); ++it)
		{
			if (it->second && it->second->m_isReadyForRender && it->second->m_isGenerating == 0)
			{
				it->second->Render(player->m_camera.get());
				//it->second->RenderDebug(player->m_camera.get());
				num++;
			}
		}
	}
}