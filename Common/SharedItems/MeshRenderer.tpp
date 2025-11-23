#pragma once

#include "Mesh.h"
#include "MeshRenderer.h"
#include "glh.h"

template <typename Vertex>
MeshRenderer<Vertex>::MeshRenderer()
{
	glGenVertexArrays(1, &m_vaoid);
	glBindVertexArray(m_vaoid);
	m_vbo.Bind();
	m_ebo.Bind();
	Vertex::SetLayout(&m_vbo.Layout());
}

template <typename Vertex>
void MeshRenderer<Vertex>::UseRendererSystem(Renderer* renderer)
{
	m_rendererSystem = renderer;
}

template <typename Vertex>
void MeshRenderer<Vertex>::UseMesh(Mesh<Vertex>* mesh)
{
	m_mesh = mesh;
}

template <typename Vertex>
void MeshRenderer<Vertex>::UpdateBuffers()
{
	m_vbo.LinkExternal(&m_mesh->GetVertices(), m_mesh->GetVerticesCount() * sizeof(Vertex));
	m_ebo.LinkExternal(&m_mesh->GetIndices(), m_mesh->GetIndicesCount() * sizeof(unsigned int));
}

template <typename Vertex>
void MeshRenderer<Vertex>::Render(Camera* camera)
{
	m_rendererSystem->Push(RenderRequest<Vertex>(this, camera));
}

template <typename Vertex>
void MeshRenderer<Vertex>::BindAtlas(unsigned int id)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}
