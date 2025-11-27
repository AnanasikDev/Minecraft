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
MeshRenderer<Vertex>::~MeshRenderer()
{
	if (m_vaoid != 0)
	{
		glDeleteVertexArrays(1, &m_vaoid);
		m_vaoid = 0;
	}
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
	glBindVertexArray(m_vaoid);
	while (glGetError());
	//printf("Updating buffers v:%d i:%d\n", m_mesh->GetVerticesCount(), m_mesh->GetIndicesCount());

	m_vbo.LinkExternal(&m_mesh->GetVertices(), m_mesh->GetVerticesCount() * Vertex::GetStride());
	m_ebo.LinkExternal(&m_mesh->GetIndices(), m_mesh->GetIndicesCount() * sizeof(unsigned int));
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error: " << err << std::endl;
	}
	glBindVertexArray(0);
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
