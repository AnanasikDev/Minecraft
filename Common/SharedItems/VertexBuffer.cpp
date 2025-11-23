#include "VertexBuffer.h"
#include "stb_image/stb_image.h"

//void VertexBuffer::LinkExternal(std::vector<Vertex>* buffer, unsigned int initSize)
//{
//	m_bufferPtr = buffer;
//	Bind();
//	glBufferData(GL_ARRAY_BUFFER, initSize, m_bufferPtr->data(), GL_STATIC_DRAW);
//}
//
//void VertexBuffer::EditSubData(unsigned int shift, unsigned int size, void* data)
//{
//	Bind();
//	glBufferSubData(GL_ARRAY_BUFFER, shift, size, data);
//}
//
//VertexBuffer::VertexBuffer()
//{
//	glGenBuffers(1, &m_id);
//}
//
//void VertexBuffer::Bind()
//{
//	glBindBuffer(GL_ARRAY_BUFFER, m_id);
//	printf("Bound VBO %d\n", m_id);
//}
//
//void VertexBuffer::Unbind()
//{
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	printf("Unbound VBO\n");
//}