#pragma once

#include "glh.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include "Program.h"
#include "Game.h"
#include "Camera.h"
#include "IRenderable.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "Renderer.h"
#include "Mesh.h"

template <typename Vertex>
void Renderer::Push(RenderRequest<Vertex> request)
{
	glBindVertexArray(request.m_vao);

	request.m_matModel = glm::translate(request.m_matModel, glm::vec3(1.f, 0.0f, 0.0f));

	unsigned int umodel = glGetUniformLocation(m_game->program->shaderProgram, "u_ModelMat");
	glUniformMatrix4fv(umodel, 1, GL_FALSE, glm::value_ptr(request.m_matModel));

	unsigned int uview = glGetUniformLocation(m_game->program->shaderProgram, "u_ViewMat");
	glUniformMatrix4fv(uview, 1, GL_FALSE, glm::value_ptr(request.m_matView));

	unsigned int uproj = glGetUniformLocation(m_game->program->shaderProgram, "u_ProjMat");
	glUniformMatrix4fv(uproj, 1, GL_FALSE, glm::value_ptr(request.m_matProjection));

	unsigned int mode = m_mode;
	if (request.m_modeOverride.has_value())
		mode = request.m_modeOverride.value();

	glDrawElements(mode, request.m_ebo->GetLength(), GL_UNSIGNED_INT, nullptr);
}

