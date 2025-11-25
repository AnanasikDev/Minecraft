#pragma once

#include "glh.h"
#include <stdlib.h>

struct Shader;

struct Program
{
	GLuint id = 0;
	GLuint shaderProgram = 0;

	Program()
	{
		id = glCreateProgram();
	}

	template <int num>
	bool AddShaders(Shader* shaders[num])
	{
		shaderProgram = glCreateProgram();
		for (int i = 0; i < num; i++)
		{
			glAttachShader(shaderProgram, shaders[i]->id);
		}
		glLinkProgram(shaderProgram);
		glValidateProgram(shaderProgram);

		for (int i = 0; i < num; i++)
		{
			glDeleteShader(shaders[i]->id);
		}

		int success = 0;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			int length = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length * sizeof(char));
			glGetProgramInfoLog(shaderProgram, length, &length, message);
			free(message);
		}
		return success;
	}

	void Use()
	{
		glLinkProgram(shaderProgram);
	}
};