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

	Block::ID m_id{	Block::ID::Air };
	unsigned char m_light : 4;
	unsigned char m_skyExposure : 4;

	Block() : m_id(ID::Air), m_light(0), m_skyExposure(0)
	{
	}
	Block(Block::ID id) : m_id(id), m_light(0), m_skyExposure(0)
	{
	}
	Block(Block::ID id, unsigned char light) : m_id(id), m_light(light), m_skyExposure(0)
	{
	}

	inline Block::ID GetID() const
	{
		return m_id;
	}
	
	struct BlockData* GetData() const;


	void Set(Block::ID id, bool updateLight = true);

	inline unsigned char GetLightLevel() const
	{
		return m_light;
	}
	
	inline void SetLightLevel(unsigned char light)
	{
		m_light = light;
	}

	unsigned char GetEmission() const;
	bool IsLightEmitter() const;

	static inline unsigned char GetLightLevelSafe(const Block* block)
	{
		if (!block) return 15;
		return block->GetLightLevel();
	}

	inline void SetSkyExposure(unsigned char newval)
	{
		m_skyExposure = newval;
	}

	inline unsigned char GetSkyExposure() const
	{
		return m_skyExposure;
	}

	inline unsigned char GetTotalLightPacked() const
	{
		return (std::max(m_light, GetEmission()) << 4) | (m_skyExposure & 0xf);
	}
};
