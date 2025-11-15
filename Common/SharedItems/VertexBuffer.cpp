#include "VertexBuffer.h"
#include "stb_image/stb_image.h"

void VertexBuffer::LinkExternalVertexBuffer(void* buffer, unsigned int size, GLenum mode)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, buffer, mode);
	m_buffer = reinterpret_cast<FVertex*>(buffer);
}

void VertexBuffer::CreateVertexBuffer(unsigned int initSize, GLenum mode)
{
	Bind();
	m_buffer = (FVertex*)malloc(initSize);
	glBufferData(GL_ARRAY_BUFFER, initSize, m_buffer, mode);
}

void VertexBuffer::EditSubData(unsigned int shift, unsigned int size, void* data)
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, shift, size, data);
}

void VertexBuffer::ReadAtlas(unsigned int id)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &m_id);
}

void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	printf("Bound VBO %d\n", m_id);
}

void VertexBuffer::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	printf("Unbound VBO\n");
}