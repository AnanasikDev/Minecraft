#include "TextureAtlas.h"
#include "glh.h"
#include "stb_image/stb_image.h"

void TextureAtlas::Create3D(std::string path, unsigned int wrapMode, int tilesize)
{
	m_is3D = true;
	glGenTextures(1, &m_id);
	Use();
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* data = stbi_load(path.c_str(), &m_widthPixels, &m_heightPixels, &m_numChannels, 0);
	int format = GL_RGBA;

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, tilesize, tilesize, m_heightPixels / tilesize, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	stbi_image_free(data);
}

void TextureAtlas::Create2D(std::string path, unsigned int wrapMode, int tilesize)
{
	m_is3D = false;
	glGenTextures(1, &m_id);
	Use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* data = stbi_load(path.c_str(), &m_widthPixels, &m_heightPixels, &m_numChannels, 0);
	int format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_widthPixels, m_heightPixels, 0, format, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

void TextureAtlas::Use()
{
	if (m_is3D)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
}

UVRect TextureAtlas::GetUVs(int index) const
{
	const int columns	{ m_widthPixels / m_tileSize };
	const int x			{ index % columns };
	const int y			{ index / columns };
	const float x_px	{ static_cast<float>(x * m_tileSize) };
	const float y_px	{ static_cast<float>(y * m_tileSize) };

	// To prevent bleeding
	constexpr float offset{ 0.5f };
	const float width{ static_cast<float>(m_widthPixels) };
	const float height{ static_cast<float>(m_heightPixels) };

	return {
		glm::vec2((x_px + offset) / width,
			   (y_px + offset) / height),

		glm::vec2((x_px + m_tileSize - offset) / width,
			   (y_px + m_tileSize - offset) / height)
	};
}

UVRect Texture::GetUVs(int index, int tileSize) const
{
	const int columns	{ m_widthPixels / tileSize };
	const int x			{ index % columns };
	const int y			{ index / columns };
	const float x_px	{ static_cast<float>(x * tileSize) };
	const float y_px	{ static_cast<float>(y * tileSize) };

	// To prevent bleeding
	constexpr float offset{ 0.5f };
	const float width{ static_cast<float>(m_widthPixels) };
	const float height{ static_cast<float>(m_heightPixels) };

	return {
		glm::vec2((x_px + offset) / width,
			   (y_px + offset) / height),

		glm::vec2((x_px + tileSize - offset) / width,
			   (y_px + tileSize - offset) / height)
	};
}

// =========================================

void Texture::Create(std::string path)
{
	glGenTextures(1, &m_id);
	Use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* data = stbi_load(path.c_str(), &m_widthPixels, &m_heightPixels, &m_numChannels, 0);
	int format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_widthPixels, m_heightPixels, 0, format, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

void Texture::Use()
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}