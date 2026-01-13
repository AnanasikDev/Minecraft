#pragma once

#include <functional>
#include "commons.h"
#include <algorithm>
#include <vector>
#include <cctype>
#include <string>
#include <unordered_map>
#include "glm/vec3.hpp"
#include "Interaction.h"

struct Collider;

struct Block
{
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
		TNT,
		LAST = TNT,

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
	Block(Block::ID id, unsigned char light) : m_id(id), m_light(light & 0xF), m_skyExposure(0)
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
		m_light = light & 0xF;
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
		m_skyExposure = newval & 0xF;
	}

	inline unsigned char GetSkyExposure() const
	{
		return m_skyExposure;
	}

	inline unsigned char GetTotalLightPacked() const
	{
		return (std::max(m_light, GetEmission()) << 4) | (m_skyExposure & 0xf);
	}

	Collider* GetCollider();

	Interaction::Result TryInteract(Interaction args) const;

	static constexpr Block::ID IntToID(int _int)
	{
		if (_int <= static_cast<int>(Block::ID::LAST) && _int >= 0)
			return static_cast<Block::ID>(_int);
		else
			return Block::ID::INVALID;
	}

	static Block::ID NameToID(std::string name)
	{
		std::transform(name.begin(), name.end(), name.begin(),
			[](unsigned char c) { return std::tolower(c); });

		if (name == "air")        return Block::ID::Air;
		if (name == "dirt")       return Block::ID::Dirt;
		if (name == "grass")      return Block::ID::Grass;
		if (name == "stone")      return Block::ID::Stone;
		if (name == "oaklog")     return Block::ID::OakLog;
		if (name == "oakleaves")  return Block::ID::OakLeaves;
		if (name == "sand")       return Block::ID::Sand;
		if (name == "bedrock")    return Block::ID::Bedrock;
		if (name == "water")      return Block::ID::Water;
		if (name == "firlog")     return Block::ID::FirLog;
		if (name == "firleaves")  return Block::ID::FirLeaves;
		if (name == "lamp")       return Block::ID::Lamp;
		if (name == "tnt")        return Block::ID::TNT;

		return Block::ID::INVALID;
	}
};
