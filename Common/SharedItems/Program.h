#pragma once

#include <string>
#include <stdlib.h>
#include "glh.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

struct Shader;

struct Program
{
	GLuint id = 0;

	Program()
	{
		id = glCreateProgram();
	}

	~Program()
	{
		if (id) glDeleteProgram(id);
	}

	template <int num>
	bool AddShaders(Shader* shaders[num])
	{
		for (int i = 0; i < num; i++)
		{
			glAttachShader(id, shaders[i]->id);
		}
		glLinkProgram(id);
		glValidateProgram(id);

		for (int i = 0; i < num; i++)
		{
			glDeleteShader(shaders[i]->id);
			delete shaders[i];
		}

		int success = 0;
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success)
		{
			int length = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length * sizeof(char));
			glGetProgramInfoLog(id, length, &length, message);
			free(message);
		}
		return success;
	}

	void Use()
	{
		glUseProgram(id);
	}

	void SetMatrices(glm::mat4 model, glm::mat4 viewProjection)
	{
		unsigned int loc_ModelMat = glGetUniformLocation(id, "u_ModelMat");
		glUniformMatrix4fv(loc_ModelMat, 1, GL_FALSE, glm::value_ptr(model));

		unsigned int loc_VPMat = glGetUniformLocation(id, "u_VPMat");
		glUniformMatrix4fv(loc_VPMat, 1, GL_FALSE, glm::value_ptr(viewProjection));
	}

	void SetFloat(std::string id, float value)
	{
		unsigned int loc = glGetUniformLocation(this->id, id.c_str());
		glUniform1f(loc, value);
	}
};