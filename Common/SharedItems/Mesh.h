#pragma once

#include <vector>
#include <memory>
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "Transform.h"
#include "IRenderable.h"
#include "Camera.h"
#include "RenderRequest.h"

template <typename Vertex>
struct Mesh
{
	std::unique_ptr<std::vector<Vertex>> m_vertices;
	std::unique_ptr<std::vector<unsigned int>> m_indices;

	Mesh()
	{
		m_vertices = std::make_unique<std::vector<Vertex>>();
		m_indices = std::make_unique<std::vector<unsigned int>>();
	}
	~Mesh() = default;
	Mesh(Mesh&& other) = delete;
	Mesh(const Mesh& other) = delete;

	void AddVertices(Vertex* data, int count)
	{
		m_vertices->reserve(m_vertices->size() + count);
		for (int i = 0; i < count; i++)
		{
			m_vertices->push_back(data[i]);
		}
	}

	void AddIndices(unsigned int* data, int count)
	{
		m_indices->reserve(m_indices->size() + count);
		for (int i = 0; i < count; i++)
		{
			m_indices->push_back(data[i]);
		}
	}

	void AddFace(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4, unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4, unsigned int i5, unsigned int i6)
	{
		m_vertices->push_back(v1);
		m_vertices->push_back(v2);
		m_vertices->push_back(v3);
		m_vertices->push_back(v4);

		m_indices->push_back(i1);
		m_indices->push_back(i2);
		m_indices->push_back(i3);
		m_indices->push_back(i4);
		m_indices->push_back(i5);
		m_indices->push_back(i6);
	}

	inline int GetVerticesCount() const
	{
		return static_cast<int>(m_vertices->size());
	}
	inline int GetIndicesCount() const
	{
		return static_cast<int>(m_indices->size());
	}
	void ClearGeometry()
	{
		m_vertices->clear();
		m_indices->clear();
	}
	
	inline std::vector<Vertex>& GetVertices()
	{
		return *m_vertices.get();
	}
	inline std::vector<unsigned int>& GetIndices()
	{
		return *m_indices.get();
	}
};