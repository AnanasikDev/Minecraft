#pragma once

#include <functional>
#include "commons.h"
#include <vector>
#include <unordered_map>
#include "glm/vec3.hpp"

struct Block
{
	static constexpr Block* Invalid = nullptr;

	enum class ID : unsigned char
	{
		Air = 0,
		Dirt,
		Grass,
		Stone,
		OakLog,
		OakLeaves,
		Sand,
		Bedrock,
		Water,
		FirLog,
		FirLeaves,
		Lamp,

		INVALID = 255
	};

	Block::ID m_id;
	char m_light;

	Block() : m_id(ID::Air), m_light(0)
	{
	}
	Block(Block::ID id) : m_id(id), m_light(0)
	{
	}

	inline Block::ID GetID() const
	{
		return m_id;
	}
	
	struct BlockData* GetData() const;


	void Set(Block::ID id);

	inline char GetLightLevel() const
	{
		return m_light;
	}
	
	void SetLightLevel(char light)
	{
		m_light = light;
	}

	static inline char GetLightLevelSafe(const Block* block)
	{
		if (!block) return 15;
		return block->GetLightLevel();
	}
};
