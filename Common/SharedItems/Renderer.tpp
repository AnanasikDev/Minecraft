#pragma once

#include "glh.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include "Shader.h"
#include "Program.h"
#include "Game.h"
#include "Camera.h"
#include "IRenderable.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "Renderer.h"
#include "Mesh.h"
#include "BaseDebug.h"

template <typename Vertex>
void Renderer::Push(RenderRequest<Vertex> request)
{
	//while (glGetError());
	START_ERROR_CAPTURE();

	glBindVertexArray(request.m_vao);

	//request.m_matModel = glm::translate(request.m_matModel, glm::vec3(1.f, 0.0f, 0.0f));

	unsigned int umodel = glGetUniformLocation(m_game->program->shaderProgram, "u_ModelMat");
	glUniformMatrix4fv(umodel, 1, GL_FALSE, glm::value_ptr(request.m_matModel));

	unsigned int uview = glGetUniformLocation(m_game->program->shaderProgram, "u_ViewMat");
	glUniformMatrix4fv(uview, 1, GL_FALSE, glm::value_ptr(request.m_matView));

	unsigned int uproj = glGetUniformLocation(m_game->program->shaderProgram, "u_ProjMat");
	glUniformMatrix4fv(uproj, 1, GL_FALSE, glm::value_ptr(request.m_matProjection));

	//unsigned int umaxlight = glGetUniformLocation(m_game->program->shaderProgram, "u_maxLightLevel");
	//glUniform1i(uproj, 16);

	unsigned int mode = m_mode;
	if (request.m_modeOverride != RENDER_MODE::DEFAULT_MODE)
		mode = request.m_modeOverride;

	glDrawElements(mode, request.m_ebo->GetLength(), GL_UNSIGNED_INT, nullptr);

	/*GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error: " << err << std::endl;
	}*/

	END_ERROR_CAPTURE();

	VertexBuffer<Vertex>::NUM_RENDERED += request.m_vbo->GetLength();
}