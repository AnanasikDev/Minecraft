#include <stdlib.h>
#include <stdio.h>
#include "ElementBuffer.h"

ElementBuffer::ElementBuffer()
{
	glGenBuffers(1, &m_id);
}

void ElementBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	printf("Bound EBO %d\n", m_id);
}

void ElementBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	printf("Unbound EBO\n");
}

void ElementBuffer::LinkExternalElementBuffer(void* buffer, unsigned int size, GLenum mode)
{
	Bind();
	m_buffer = (GLuint*)buffer;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, m_buffer, mode);
}

void ElementBuffer::CreateElementBuffer(unsigned int initSize, GLenum mode)
{
	m_buffer = (GLuint*)malloc(initSize);
}
