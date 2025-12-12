#pragma once

#include <string>
#include "BufferLayout.h"
#include "Vertex.h"

template <typename Vertex>
class VertexBuffer
{
private:
	BufferLayout m_layout;
	GLuint m_id{ 0 };
	std::vector<Vertex>* m_bufferPtr{ nullptr };

public:
	
	static int NUM_RENDERED;

	VertexBuffer()
	{
		glGenBuffers(1, &m_id);
	}

	~VertexBuffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void LinkExternal(std::vector<Vertex>* buffer, unsigned int initSize)
	{
		m_bufferPtr = buffer;
		Bind();
		glBufferData(GL_ARRAY_BUFFER, initSize, m_bufferPtr->data(), GL_STATIC_DRAW);
	}

	void EditSubData(unsigned int shift, unsigned int size, void* data)
	{
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, shift, size, data);
	}

	void Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	inline float* GetRawData()
	{
		return reinterpret_cast<float*>(m_bufferPtr->data());
	}

	inline int GetLength() const
	{
		return m_bufferPtr->size();
	}

	inline Vertex* GetVertexData()
	{
		return m_bufferPtr->data();
	}

	inline BufferLayout& Layout()
	{
		return m_layout;
	}

	inline GLuint* GetVBOIDp() 
	{
		return &m_id;
	}
	inline GLuint GetVBOID() const
	{
		return m_id;
	}
};

template <typename V>
int VertexBuffer<V>::NUM_RENDERED = 0;
