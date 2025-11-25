#pragma once

#include <functional>
#include "commons.h"
#include <vector>
#include <unordered_map>
#include "glm/vec3.hpp"

class Chunk;

struct Context
{
	glm::ivec3 pos;
	Chunk* const chunk;
	GridVec vec;

	Context(glm::ivec3 pos, GridVec vec, Chunk* const chunk) : pos(pos), vec(vec), chunk(chunk)
	{
	}
};

struct BlockData;

class Block
{

public:
	static constexpr Block* Invalid = nullptr;

	BlockData* m_data = nullptr;

	enum class ID : unsigned char
	{
		Air = 0,
		Dirt,
		Grass,
		Stone,

		INVALID = 255
	};

	Block(BlockData* data);
	Block() = default;

	bool IsSolid() const;
	bool IsAir() const;
	bool IsValid() const;

	void Set(BlockData* data);
};
