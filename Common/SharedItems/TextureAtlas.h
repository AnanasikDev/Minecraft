#pragma once

#include <string>
#include "Block.h"

class TextureAtlas
{
public:
	unsigned int m_id;

	enum class TextureID : int
	{
		Dirt = 0,
		GrassBlockSide,
		GrassBlockTop,
		Stone,
	};

	TextureID GetTextureIDByBlock(Block::ID id, GridVec face /* state, etc */);

	void BindAtlas(std::string path, unsigned int wrapMode);
};