#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "Transform.h"
#include "IRenderable.h"
#include "Camera.h"
#include "RenderRequest.h"
#include "Face.h"

template <typename Vertex>
struct MeshBase
{
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	MeshBase() = default;
	~MeshBase() = default;
	MeshBase(MeshBase&& other) = delete;
	MeshBase(const MeshBase& other) = delete;
	MeshBase& operator=(const MeshBase& other) = delete;
	MeshBase& operator=(MeshBase&& other) = delete;

	void CopyFrom(const MeshBase& other)
	{
		//m_vertices = other.m_vertices;
		//m_indices = other.m_indices;

		m_vertices.insert(m_vertices.end(), other.m_vertices.begin(), other.m_vertices.end());
		m_indices.insert(m_indices.end(), other.m_indices.begin(), other.m_indices.end());
	}

	void AddVertices(Vertex* data, int count)
	{
		for (int i = 0; i < count; i++)
		{
			m_vertices.push_back(data[i]);
		}
	}

	void AddIndices(unsigned int* data, int count)
	{
		for (int i = 0; i < count; i++)
		{
			m_indices.push_back(data[i]);
		}
	}

	void AddFace(Face<Vertex>& face)
	{
		m_vertices.insert(m_vertices.end(), face.m_vertices.begin(), face.m_vertices.end());
		m_indices.insert(m_indices.end(), face.m_indices.begin(), face.m_indices.end());
	}

	inline int GetVerticesCount() const
	{
		return static_cast<int>(m_vertices.size());
	}
	inline int GetIndicesCount() const
	{
		return static_cast<int>(m_indices.size());
	}
	void ClearGeometry()
	{
		m_vertices.clear();
		m_indices.clear();
	}
	
	inline std::vector<Vertex>& GetVertices()
	{
		return m_vertices;
	}
	inline std::vector<unsigned int>& GetIndices()
	{
		return m_indices;
	}

	void Modify(std::function<void(Vertex&)> fn)
	{
		for (int i = 0; i < GetVerticesCount(); i++)
		{
			fn(m_vertices[i]);
		}
	}
};

template <typename Vertex>
struct Mesh : public MeshBase<Vertex>
{

};

template <>
struct Mesh<DebugVertex> : public MeshBase<DebugVertex>
{
	static Mesh<DebugVertex> MESH_BOX;
};

template <>
struct Mesh<FVertex> : public MeshBase<FVertex>
{
	static Mesh<FVertex> MESH_BOX;
};