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
		OakLogSide,
		OakLogTop,
		OakLeaves,
		Sand,
		Bedrock,
		Water,
		FirLogSide,
		FirLogTop,
		FirLeaves,
		Lamp,
	};

	void BindAtlas(std::string path, unsigned int wrapMode);
};