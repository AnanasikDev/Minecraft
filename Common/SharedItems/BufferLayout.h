#pragma once

#include "commons.h"

class BufferLayout
{
public:
	template <typename T>
	void PushAttribute(unsigned int count, size_t stride)
	{
		_pushAttribute<GetGLEnumType<T>(), static_cast<int>(sizeof(T))>(count, stride);
	}

private:
	int m_position{ 0 };
	int m_start{ 0 };

	template <GLenum type, int size>
	void _pushAttribute(unsigned int count, size_t stride)
	{
		if constexpr (IsGLEnumTypeInt(type))
		{
			glVertexAttribIPointer(m_position, count, type, static_cast<GLint>(stride), reinterpret_cast<void*>(static_cast<uintptr_t>(m_start)));
		}
		else
		{
			glVertexAttribPointer(m_position, count, type, GL_FALSE, static_cast<GLint>(stride), reinterpret_cast<void*>(static_cast<uintptr_t>(m_start)));
		}
		glEnableVertexAttribArray(m_position);
		m_position++;
		m_start += static_cast<int>(count) * size;
	}
};