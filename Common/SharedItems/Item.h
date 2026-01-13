#pragma once
#include <glm/vec2.hpp>
#include "TextureAtlas.h"
#include <optional>
#include "Block.h"

class Item
{
public:
	static constexpr float UI_SIZE{ 50 };

	enum class ID : short
	{
		None = -1,
		DirtBlock,
		GrassBlock,
		StoneBlock,
		OakLogBlock,
		OakLeavesBlock,
		SandBlock,
		BedrockBlock,
		WaterBlock,
		FirLogBlock,
		FirLeavesBlock,
		LampBlock,
		TNTBlock,
	};

	Item::ID m_id{ ID::None };
	TextureAtlas* m_atlas{ nullptr };

	static constexpr int IntID(ID id)
	{
		return static_cast<int>(id);
	}

	Item() = default;
	Item(ID id, TextureAtlas* atlas) : m_id(id), m_atlas(atlas)
	{
	}

	void Render(glm::vec2 position, float scale);

	static std::optional<Block::ID> ItemToBlock(Item::ID itemid);
	static std::optional<Item::ID> BlockToItem(Block::ID itemid);
};