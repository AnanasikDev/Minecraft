#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <atomic>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include <unordered_map>
#include "PerlinNoise.hpp"
#include "Chunk.h"
#include "SafeQueue.h"

class Game;
class Player;

class World
{
public:
	Game* m_game{ nullptr };
	static constexpr float BASE_NOISE_SCALE{ 0.05f };
	static constexpr float CAVES_NOISE_SCALE{ 0.075f };
	static constexpr float MOUNTAINS_NOISE_SCALE{ 0.02f };
	static constexpr int GENERATION_DISTANCE{ 5 };

	World();
	~World();
	
	void AsyncUpdate();

	void Init(Game* game);
	void Update();
	void FixedUpdate();
	float GetHeightAt(int x, int z);
	float GetDensityAt(int x, int y, int z);
	void Render(Player* player);

	void GenerateChunkAt(glm::ivec2 chunkPos);

private:
	siv::PerlinNoise::seed_type m_seed{ 123456u };
	siv::PerlinNoise m_perlin{ m_seed };
	std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>> m_chunks;
	std::mutex m_chunksMutex;
	std::atomic<bool> m_isRunning;
	SafeQueue<Chunk*> m_togen;
	std::thread m_workerThread01;
	std::thread m_workerThread02;
	std::thread m_workerThread03;
};