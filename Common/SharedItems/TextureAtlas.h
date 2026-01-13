#pragma once

#include <string>
#include "Block.h"

struct UVRect
{
	glm::vec2 uv0;
	glm::vec2 uv1;
};

class TextureAtlas
{
public:
	unsigned int m_id{ 0 };
	int m_widthPixels{ 0 };
	int m_heightPixels{ 0 };
	int m_numChannels{ 0 };
	int m_tileSize{ 16 };
	bool m_is3D{ true };
	
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
		TNTBottom,
		TNTSide,
		TNTTop,
	};

	void Create3D(std::string path, unsigned int wrapMode, int tilesize);
	void Create2D(std::string path, unsigned int wrapMode, int tilesize);
	void Use();
	UVRect GetUVs(int index) const;
};

class Texture
{
public:
	unsigned int m_id{ 0 };
	int m_widthPixels{ 0 };
	int m_heightPixels{ 0 };
	int m_numChannels{ 0 };

	void Create(std::string path);
	void Use();
	UVRect GetUVs(int index, int tileSize) const;
};