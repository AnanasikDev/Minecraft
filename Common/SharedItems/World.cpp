#include "World.h"
#include "Game.h"
#include "Player.h"
#include "Camera.h"
#include "Random.h"

World::World()
{
	m_isRunning = true;
	m_workerThread01 = std::thread(&World::AsyncUpdate, this);
	m_workerThread02 = std::thread(&World::AsyncUpdate, this);
	m_workerThread03 = std::thread(&World::AsyncUpdate, this);
}

World::~World()
{
	m_isRunning = false;
	if (m_workerThread01.joinable())
	{
		m_workerThread01.join();
	}
	if (m_workerThread02.joinable())
	{
		m_workerThread02.join();
	}
	if (m_workerThread03.joinable())
	{
		m_workerThread03.join();
	}
}

void World::Init(Game* game)
{
	m_game = game;
}

void World::Update()
{
	const glm::ivec2 chunkPos = m_game->m_player->m_transform.GetChunkPosition();
	{
		std::lock_guard<std::mutex> mapLock(m_chunksMutex);
		for (int dx = -GENERATION_DISTANCE; dx <= GENERATION_DISTANCE; dx++)
		{
			for (int dz = -GENERATION_DISTANCE; dz <= GENERATION_DISTANCE; dz++)
			{
				const glm::ivec2 check(chunkPos.x + dx, chunkPos.y + dz);
				
				bool isInRange{
					(glm::length(glm::vec2(dx, dz)) < GENERATION_DISTANCE + 1)
				};
				if (!isInRange) continue;

				const glm::vec3 eye{ m_game->m_player->m_transform.GetForward() };
				const glm::vec3 eyehorz(eye.x, 0, eye.z);
				bool isInFrustum{
					(glm::abs(glm::dot(eyehorz, glm::normalize(glm::vec3(dx, 0, dz))) - 1) < 0.35f) 
				};
				if (!isInFrustum) continue;
	
				if (m_chunks.find(check) == m_chunks.end())
				{
					GenerateChunkAt(check);
				}
				Chunk* chunk = m_chunks.at(check).get();
				if (chunk->m_isDirty)
				{
					chunk->m_isDirty = false;
					chunk->m_isReadyForRender = false;
					chunk->m_workersAccess++;
					m_togen.push(chunk);
				}
				else if (chunk->m_isGPUDirty)
				{
					chunk->UpdateGPUBuffers();
				}
			}
		}
	}
}

void World::FixedUpdate()
{
	std::lock_guard<std::mutex> mapLock(m_chunksMutex);
	std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>>::iterator it;
	glm::ivec2 playerChunk = m_game->m_player->m_transform.GetChunkPosition();
	for (it = m_chunks.begin(); it != m_chunks.end();)
	{
		std::unique_lock<std::mutex> lock(it->second->m_mtx);
		const float distance = glm::length(glm::vec2(it->first) - glm::vec2(playerChunk));
		if (distance > GENERATION_DISTANCE + 1 && it->second->m_isReadyForRender && !it->second->m_isGPUDirty && it->second->m_workersAccess == 0)
		{
			lock.unlock();
			it = m_chunks.erase(it);
		}
		else
		{
			lock.unlock();
			++it;
		}
	}
}

void World::AsyncUpdate()
{
	while (m_isRunning)
	{
		Chunk* chunk;
		if (m_togen.try_pop(chunk))
		{
			{
				std::lock_guard<std::mutex> lock(chunk->m_mtx);
				chunk->m_mesh.ClearGeometry();
				for (Coord x = 0; x < Chunk::XWIDTH; x++)
				{
					for (Coord z = 0; z < Chunk::ZDEPTH; z++)
					{
						for (Coord y = 0; y < Chunk::YHEIGHT; y++)
						{
							Block* block = chunk->At(x, y, z);
							if (!block || block->IsAir()) continue;

							chunk->GenerateBlock(x, y, z, *block);
						}
					}
				}
			}
			chunk->m_workersAccess--;
			chunk->m_isGPUDirty = true;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void World::GenerateChunkAt(glm::ivec2 chunkPos)
{
	/*m_chunks.emplace(std::piecewise_construct,
		std::forward_as_tuple(chunkPos),
		std::forward_as_tuple(chunkPos, m_game));*/
	m_chunks.try_emplace(chunkPos, std::make_unique<Chunk>(chunkPos, m_game));
}

float World::GetHeightAt(int x, int z)
{
	float mountains = fabsf(powf(m_perlin.noise2D(x * MOUNTAINS_NOISE_SCALE, z * MOUNTAINS_NOISE_SCALE), 3.5f)) * 0.0f;
	return 0.45f + m_perlin.noise2D(x * BASE_NOISE_SCALE, z * BASE_NOISE_SCALE) / 7.0f + Random::GetFloat(-0.0075f, 0.0075f);// +mountains;
	//return fabsf(powf(m_perlin.noise2D(x * MOUNTAINS_NOISE_SCALE, z * MOUNTAINS_NOISE_SCALE), 3.5f) * 4.0f);
}

float World::GetDensityAt(int x, int y, int z)
{
	return m_perlin.noise3D(x * CAVES_NOISE_SCALE, y * CAVES_NOISE_SCALE, z * CAVES_NOISE_SCALE);
}

void World::Render(Player* player)
{
	{
		std::lock_guard<std::mutex> mapLock(m_chunksMutex);
		std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>>::iterator it;
		int num = 0;
		for (it = m_chunks.begin(); it != m_chunks.end(); ++it)
		{
			it->second->Render(player->m_camera.get());
			//it->second.RenderDebug(player->m_camera.get());
			num++;
		}
	}
}