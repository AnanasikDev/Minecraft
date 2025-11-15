#pragma once

#include "Block.h"
#include <functional>
#include "TextureAtlas.h"
#include <optional>

struct BlockData
{
	Block::ID id;
	bool isSolid;
	std::function<void(const BlockData& data, const Context& ctx)> generateGeometry;
	
	BlockData() = default;

	BlockData(Block::ID id) : id(id)
	{

	}

	void SetGeomFunc(const std::function<void(const BlockData& data, const Context& ctx)>& generateGeometry)
	{
		this->generateGeometry = generateGeometry;
	}

	void GenerateGeometry(const Context& ctx);

	const Block* At(const Context& ctx, GridVec vec) const;
	Block::ID IDAt(const Context& ctx, GridVec vec) const;
	void Draw(const Context& ctx, TextureAtlas::TextureID textureid) const;
};
