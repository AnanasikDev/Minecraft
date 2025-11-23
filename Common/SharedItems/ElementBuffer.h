#pragma once

#include <glh.h>
#include <vector>

class ElementBuffer
{
private:
	unsigned int m_id{ 0 };
	std::vector<unsigned int>* m_bufferPtr{ nullptr };

public:
	ElementBuffer();
	~ElementBuffer()
	{
		//glDeleteBuffers(1, &m_id);
	}

	void LinkExternal(std::vector<unsigned int>* buffer, unsigned int size);
	void Bind();
	void Unbind();

	inline int GetLength() const
	{
		return static_cast<int>(m_bufferPtr->size());
	}

	inline unsigned int* GetRawData()
	{
		return m_bufferPtr->data();
	}

	inline unsigned int* GetEBOIDp()
	{
		return &m_id;
	}
	inline unsigned int GetEBOID() const
	{
		return m_id;
	}
};