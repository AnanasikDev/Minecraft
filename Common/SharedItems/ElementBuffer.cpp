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
}

void ElementBuffer::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ElementBuffer::LinkExternal(std::vector<unsigned int>* buffer, unsigned int size)
{
	Bind();
	m_bufferPtr = buffer;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, m_bufferPtr->data(), GL_STATIC_DRAW);
}