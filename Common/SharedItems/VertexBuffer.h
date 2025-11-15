#pragma once

#include "BufferLayout.h"
#include <string>
#include "Vertex.h"

class VertexBuffer
{
private:
	BufferLayout m_layout;
	GLuint m_id;
	FVertex* m_buffer = nullptr;

public:
	
	VertexBuffer();

	void LinkExternalVertexBuffer(void* buffer, unsigned int size, GLenum mode);
	void CreateVertexBuffer(unsigned int initSize, GLenum mode);
	void EditSubData(unsigned int shift, unsigned int size, void* data);
	void ReadAtlas(unsigned int id);

	void Bind();
	void Unbind();

	inline float* GetRawData()
	{
		return reinterpret_cast<float*>(m_buffer);
	}

	inline FVertex* GetVertexData()
	{
		return m_buffer;
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