#pragma once

#include "IRenderable.h"
#include "Transform.h"
#include "Mesh.h"
#include "ElementBuffer.h"
#include "VertexBuffer.h"
#include "RenderRequest.h"
#include "Renderer.h"

class Renderer;
class Camera;

template <typename Vertex>
struct Mesh;

template <typename Vertex>
class MeshRenderer : public IRenderable
{
public:
	Mesh<Vertex>* m_mesh{ nullptr };
	VertexBuffer<Vertex> m_vbo;
	ElementBuffer m_ebo;
	unsigned int m_vaoid{ 0 };
	Transform m_transform;
	Renderer* m_rendererSystem{ nullptr };
	RENDER_MODE m_mode{ DEFAULT_MODE };
	Shader* m_vertShaderOverride{ nullptr };
	Shader* m_fragShaderOverride{ nullptr };

	MeshRenderer();
	~MeshRenderer();
	MeshRenderer(const MeshRenderer& other) = delete;
	MeshRenderer(MeshRenderer&& other) = delete;
	MeshRenderer& operator=(const MeshRenderer& other) = delete;
	MeshRenderer& operator=(MeshRenderer&& other) = delete;

	void UseRendererSystem(Renderer* renderer);
	void UseMesh(Mesh<Vertex>* mesh);
	void UpdateBuffers();
	void Render(Camera* camera);
	void BindAtlas(unsigned int id);
};

#include "MeshRenderer.tpp"
