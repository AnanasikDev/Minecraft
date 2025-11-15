#include "World.h"
#include "Game.h"
#include "Player.h"
#include "Camera.h"

void World::Init(Game* game)
{
	m_game = game;
}

void World::Update()
{
	glm::vec3 playerPos = m_game->m_player->m_transform.GetPosition();
	glm::ivec2 chunkPos = glm::ivec2(static_cast<int>(roundf(playerPos.x / Chunk::XWIDTH)), static_cast<int>(roundf(playerPos.z / Chunk::ZDEPTH)));

	for (int dx = -GENERATION_DISTANCE; dx <= GENERATION_DISTANCE; dx++)
	{
		for (int dz = -GENERATION_DISTANCE; dz <= GENERATION_DISTANCE; dz++)
		{
			glm::ivec2 check(chunkPos.x + dx, chunkPos.y + dz);

			if (m_chunks.find(check) == m_chunks.end())
			{
				GenerateChunkAt(check);
			}
		}
	}
}

void World::GenerateChunkAt(glm::ivec2 chunkPos)
{
	m_chunks.try_emplace
	(
		chunkPos,
		chunkPos, m_game
	);
}

float World::GetHeightAt(int x, int y, int z)
{
	return 0.5f + m_perlin.noise2D(x * NOISE_SCALE, z * NOISE_SCALE) / 3.0f;
}

float World::GetDensityAt(int x, int y, int z)
{
	return m_perlin.noise3D(x * NOISE_SCALE, y * NOISE_SCALE, z * NOISE_SCALE);
}

void World::Render(Player* player)
{
	std::unordered_map<glm::ivec2, Chunk>::iterator it;
	int num = 0;
	for (it = m_chunks.begin(); it != m_chunks.end(); ++it)
	{
		it->second.Render(player->m_camera.get());
		num++;
	}
}