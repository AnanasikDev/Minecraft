#pragma once

#include <glh.h>

class ElementBuffer
{
private:
	GLuint m_id;
	GLuint* m_buffer = nullptr;

public:
	ElementBuffer();

	void LinkExternalElementBuffer(void* buffer, unsigned int size, GLenum mode);
	void CreateElementBuffer(unsigned int initSize, GLenum mode);

	void Bind();
	void Unbind();

	inline GLuint* GetRawData()
	{
		return m_buffer;
	}

	inline GLuint* GetEBOIDp()
	{
		return &m_id;
	}
	inline GLuint GetEBOID() const
	{
		return m_id;
	}
};