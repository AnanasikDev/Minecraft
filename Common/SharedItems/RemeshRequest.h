#pragma once;

#include <unordered_map>

class Chunk;

struct RemeshRequest
{
	static constexpr size_t CAPACITY{ 6 };

	Chunk* chunk			{ nullptr };
	Chunk* neighbourNorth	{ nullptr };
	Chunk* neighbourSouth	{ nullptr };
	Chunk* neighbourUp		{ nullptr };
	Chunk* neighbourDown	{ nullptr };
	Chunk* neighbourEast	{ nullptr };
	Chunk* neighbourWest	{ nullptr };

	RemeshRequest() = default;
	RemeshRequest(Chunk* chunk, Chunk* neighbourNorth, Chunk* neighbourSouth, Chunk* neighbourUp, Chunk* neighbourDown, Chunk* neighbourEast, Chunk* neighbourWest)
		: chunk(chunk), neighbourNorth(neighbourNorth), neighbourSouth(neighbourSouth), neighbourUp(neighbourUp), neighbourDown(neighbourDown), neighbourEast(neighbourEast), neighbourWest(neighbourWest)
	{
		SetControl();
	}

	void SetControl();
	void ResetControl();
	Chunk*& Iterate(size_t index);

	Chunk* GetChunkAt(glm::ivec3 gridpos);
	Block* GetBlockAtWorld(glm::ivec3 pos);
	bool AreAllValidXZ();
	bool AreAllGeneratedXZ();
};