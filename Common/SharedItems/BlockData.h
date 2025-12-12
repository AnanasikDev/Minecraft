#pragma once

#include <functional>
#include "Block.h"
#include "GeomContext.h"
#include "TextureAtlas.h"

struct BlockData
{
	Block::ID id;
	bool isSolid : 1;
	bool isTransparent : 1;
	bool canFall : 1;
	char emission;
	std::string name;

	std::function<void(const BlockData& data, const GeomContext& ctx)> generateGeometry;
	
	BlockData() : id(Block::ID::Air), isSolid(true), isTransparent(false), canFall(false), emission(0)
	{
	}
	BlockData(Block::ID id) : id(id), isSolid(true), isTransparent(false), canFall(false), emission(0)
	{
	}

	void SetGeomFunc(const std::function<void(const BlockData& data, const GeomContext& ctx)>& generateGeometry)
	{
		this->generateGeometry = generateGeometry;
	}

	void GenerateGeometry(const GeomContext& ctx);

	void DrawGridFace(const GeomContext& ctx, TextureAtlas::TextureID textureid) const;

	static bool IsAir(Block::ID id);
	inline bool IsAir() const
	{
		return id == Block::ID::Air;
	}
	static bool IsValid(Block::ID id);
	inline bool IsValid() const
	{
		return id != Block::ID::INVALID;
	}
	static bool IsSolid(Block::ID id);
	inline bool IsSolid() const
	{
		return isSolid;
	}
	static bool IsTransparent(Block::ID id);
	inline bool IsTransparent() const
	{
		return isTransparent;
	}
	static bool IsLightSource(Block::ID id);
	inline bool IsLightSource() const
	{
		return emission > 0;
	}

	BlockData* AffectedByGravity()
	{
		canFall = true;
		return this;
	}
	BlockData* LightSource(char power)
	{
		emission = power;
		return this;
	}
	BlockData* Transparent()
	{
		isTransparent = true;
		return this;
	}
	BlockData* SimpleTexture(TextureAtlas::TextureID textureid)
	{
		SetGeomFunc([textureid](const BlockData& block, const GeomContext& ctx)
			{
				block.DrawGridFace(ctx, textureid);
			});
		return this;
	}
	BlockData* NoGraphics()
	{
		SetGeomFunc([](const BlockData& block, const GeomContext& ctx)
			{
			});
		return this;
	}
	BlockData* ContextualTexture(const std::function<void(const BlockData& data, const GeomContext& ctx)>& generateGeometry)
	{
		SetGeomFunc(generateGeometry);
		return this;
	}
};
