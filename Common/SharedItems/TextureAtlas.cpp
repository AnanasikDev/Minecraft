#include "TextureAtlas.h"
#include "glh.h"
#include "stb_image/stb_image.h"

TextureAtlas::TextureID TextureAtlas::GetTextureIDByBlock(Block::ID id, GridVec face)
{
	switch (id)
	{
		case Block::ID::Dirt:
		{
			return TextureAtlas::TextureID::Dirt;
		}
		case Block::ID::Grass:
		{
			switch (face)
			{
				case GridVec::Right:
				case GridVec::Left:
				case GridVec::Front:
				case GridVec::Back:
					return TextureAtlas::TextureID::GrassBlockSide;
				case GridVec::Top:
					return TextureAtlas::TextureID::GrassBlockTop;
				case GridVec::Bottom:
					return TextureAtlas::TextureID::Dirt;
			}
		}
	}
}

void TextureAtlas::BindAtlas(std::string path, unsigned int wrapMode)
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	constexpr int tileSize = 16;
	int width, height, numChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, tileSize, tileSize, height / tileSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	stbi_image_free(data);
}
