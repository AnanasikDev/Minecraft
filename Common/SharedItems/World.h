#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <unordered_map>
#include "PerlinNoise.hpp"
#include "Chunk.h"

class Game;
class Player;

class World
{
public:
	Game* m_game;
	static constexpr float NOISE_SCALE = 0.15f;
	static constexpr int GENERATION_DISTANCE = 3;

	World() = default;
	~World() = default;

	void Init(Game* game);
	void Update();
	float GetHeightAt(int x, int y, int z);
	float GetDensityAt(int x, int y, int z);
	void Render(Player* player);

	void GenerateChunkAt(glm::ivec2 chunkPos);

private:
	siv::PerlinNoise::seed_type m_seed = 123456u;
	siv::PerlinNoise m_perlin{ m_seed };
	std::unordered_map<glm::ivec2, Chunk> m_chunks;
};