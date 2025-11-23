#pragma once

#include "glm/mat4x4.hpp"
#include <optional>
#include "Mesh.h"

class IRenderable;
class Game;
class Camera;

template <typename Vertex>
class VertexBuffer;
class ElementBuffer;

template <typename Vertex>
class MeshRenderer;

enum RENDER_MODE
{
	DEFAULT_MODE = 0,
	SOLID_MODE = GL_TRIANGLES,
	WIREFRAME_MODE = GL_LINES
};

template <typename Vertex>
struct RenderRequest
{
	glm::mat4 m_matModel;
	glm::mat4 m_matProjection;
	glm::mat4 m_matView;

	RENDER_MODE m_modeOverride{ RENDER_MODE::DEFAULT_MODE };
	VertexBuffer<Vertex>* m_vbo;
	ElementBuffer* m_ebo;
	unsigned int m_vao;

	RenderRequest(MeshRenderer<Vertex>* const meshRenderer, const Camera* const camera);
};

#include "RenderRequest.tpp"