#include "TextureAtlas.h"
#include "glh.h"
#include "stb_image/stb_image.h"

void TextureAtlas::Create(std::string path, unsigned int wrapMode, int tilesize)
{
	glGenTextures(1, &m_id);
	Use();
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, numChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	int format = GL_RGBA;

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, tilesize, tilesize, height / tilesize, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	stbi_image_free(data);
}

void TextureAtlas::Use()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
}