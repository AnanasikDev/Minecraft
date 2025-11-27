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
	static constexpr float BASE_NOISE_SCALE{ 0.02f };
	static constexpr float CAVES_NOISE_SCALE{ 0.075f };
	static constexpr float MOUNTAINS_NOISE_SCALE{ 0.02f };
	static constexpr int GENERATION_DISTANCE{ 3 };

	World();
	~World();
	
	void AsyncUpdate();
	void MeshChunk(Chunk* chunk);

	void Init(Game* game);
	void Update();
	void FixedUpdate();
	int GetHeightAt(glm::ivec3 pos);
	float GetDensityAt(glm::ivec3 pos);
	Block::ID GetBlockIDAt(glm::ivec3 worldpos);
	void Render(Player* player);

	Chunk* GetChunkAt(glm::ivec3 pos);
	void GenerateChunkAt(glm::ivec3 chunkPos);
	void Regenerate(glm::ivec3 pos);

private:
	siv::PerlinNoise::seed_type m_seed{ 123456u };
	siv::PerlinNoise m_perlin{ m_seed };
	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> m_chunks;
	std::mutex m_chunksMutex;
	std::atomic<bool> m_isRunning;
	SafeQueue<Chunk*> m_toMeshQueue;
	SafeQueue<Chunk*> m_toUpdateGPUBuffersQueue;
	std::vector<std::thread> m_workerThreads;
};