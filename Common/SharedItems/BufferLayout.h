#pragma once

#include "commons.h"

class BufferLayout
{
public:
	template <typename T>
	void PushAttribute(unsigned int count, unsigned int stride)
	{
		_pushAttribute<GetGLEnumType<T>(), static_cast<int>(sizeof(T))>(count, stride);
	}

private:
	int m_position = 0;
	int m_start = 0;

	template <GLenum type, int size>
	void _pushAttribute(unsigned int count, unsigned int stride)
	{
		if constexpr (type == GL_BYTE || type == GL_UNSIGNED_BYTE || type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_INT || type == GL_UNSIGNED_INT)
		{
			glVertexAttribIPointer(m_position, count, type, stride, (void*)m_start);
		}
		else
		{
			glVertexAttribPointer(m_position, count, type, GL_FALSE, stride, (void*)m_start);
		}
		glEnableVertexAttribArray(m_position);
		m_position++;
		m_start += static_cast<int>(count) * size;
	}
};