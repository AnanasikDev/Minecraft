#pragma once

#include "glh.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include "Shader.h"
#include "Program.h"
#include "Game.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "Renderer.h"
#include "Mesh.h"
#include "BaseDebug.h"

template <typename Vertex>
void Renderer::Push(RenderRequest<Vertex> request)
{
	START_ERROR_CAPTURE();

	request.m_program->Use();
	glBindVertexArray(request.m_vao);

	request.m_program->SetMatrices(request.m_matModel, request.m_matProjection * request.m_matView);

	unsigned int mode = m_mode;
	if (request.m_modeOverride != RENDER_MODE::DEFAULT_MODE)
		mode = request.m_modeOverride;

	glDrawElements(mode, request.m_ebo->GetLength(), GL_UNSIGNED_INT, nullptr);

	END_ERROR_CAPTURE();

	VertexBuffer<Vertex>::NUM_RENDERED += request.m_vbo->GetLength();
}
