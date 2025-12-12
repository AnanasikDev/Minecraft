#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <atomic>
#include "amath.h"
#include <unordered_map>
#include <glm/vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "Chunk.h"
#include "SafeQueue.h"
#include "Ray.h"
#include "WorldGen.h"

class Game;
class Player;
class WorldGen;

class World
{
public:
	Game* m_game{ nullptr };
	int GENERATION_DISTANCE{ 4 };
	int CHUNKS_RENDERED{ 0 };
	int CHUNKS_ACTIVE{ 0 };

	World();
	~World();
	
	void AsyncUpdate();
	void MeshChunk(RemeshRequest& request);
	bool ShouldGenerate(glm::ivec3 chunkPos, glm::ivec3 playerChunkPos);

	void Init(Game* game);
	void Update();
	void FixedUpdate();
	
	void Render(Player* player);
	bool ForceSetBlockAtWorld(glm::ivec3 pos, Block::ID id);
	bool SetAndUpdateBlockAtWorld(glm::ivec3 pos, Block::ID id);
	Block* GetBlockAtWorld(glm::ivec3 pos);
	Chunk* GetChunkAt(glm::ivec3 pos);
	void GenerateChunkAt(glm::ivec3 chunkPos);
	void Regenerate(Chunk* chunk);
	void GenerateChunkGrid(RemeshRequest& request);

	void UpdateLightBlock(glm::ivec3 worldpos, char intensity);

	RaycastResult Raycast(Ray ray);
	RaycastResult SimpleRaycast(Ray ray, float step, bool invert);

	std::vector<Chunk*> GetNeighbours(Chunk* chunk);

	static inline constexpr glm::ivec3 SnapToBlock(glm::vec3 pos)
	{
		return glm::ivec3(
			static_cast<int>(floorf(pos.x)),
			static_cast<int>(floorf(pos.y)),
			static_cast<int>(floorf(pos.z))
		);
	}

	static inline constexpr glm::ivec3 WorldBlockToChunkGrid(glm::ivec3 blockPos)
	{
		return glm::ivec3(
			static_cast<int>(floorf(static_cast<float>(blockPos.x) / Chunk::XWIDTH)),
			static_cast<int>(floorf(static_cast<float>(blockPos.y) / Chunk::YHEIGHT)),
			static_cast<int>(floorf(static_cast<float>(blockPos.z) / Chunk::ZDEPTH))
		);
	}

	static inline constexpr glm::ivec3 ChunkGridToWorldBlock(glm::ivec3 chunkPos)
	{
		return glm::ivec3(
			chunkPos.x * Chunk::XWIDTH,
			chunkPos.y * Chunk::YHEIGHT,
			chunkPos.z * Chunk::ZDEPTH
		);
	}

	static inline constexpr glm::ivec3 WorldToLocalAny(glm::ivec3 blockPos)
	{
		return glm::ivec3(AMath::PositiveMod(blockPos.x, Chunk::XWIDTH), AMath::PositiveMod(blockPos.y, Chunk::YHEIGHT), AMath::PositiveMod(blockPos.z, Chunk::ZDEPTH));
	}

	static constexpr bool IsLocalBlockOnChunkEdge(glm::ivec3 pos)
	{
		return (pos.y == Chunk::YHEIGHT - 1) || (pos.y == 0) || (pos.x == Chunk::XWIDTH - 1) || (pos.x == 0) || (pos.z == Chunk::ZDEPTH - 1) || (pos.z == 0);
	}

	static constexpr bool IsLocalBlockInsideChunk(glm::ivec3 pos)
	{
		return (pos.y < Chunk::YHEIGHT) && (pos.y >= 0) && (pos.x < Chunk::XWIDTH) && (pos.x >= 0) && (pos.z < Chunk::ZDEPTH) && (pos.z >= 0);
	}

	static constexpr bool IsWorldBlockInsideChunk(glm::ivec3 chunkGridPos, glm::ivec3 pos)
	{
		pos -= ChunkGridToWorldBlock(chunkGridPos);
		return (pos.y == Chunk::YHEIGHT - 1) || (pos.y == 0) || (pos.x == Chunk::XWIDTH - 1) || (pos.x == 0) || (pos.z == Chunk::ZDEPTH - 1) || (pos.z == 0);
	}

	WorldGen* GetGenerator() const;
	void SetGenerator(std::unique_ptr<WorldGen> worldgen);

private:
	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> m_chunks;
	std::mutex m_chunksMutex;
	std::atomic<bool> m_isRunning;
	SafeQueue<RemeshRequest> m_toMeshQueue;
	SafeQueue<Chunk*> m_toUpdateGPUBuffersQueue;
	std::vector<std::thread> m_workerThreads;
	std::unique_ptr<WorldGen> m_generator;
};