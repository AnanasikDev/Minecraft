#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"

struct Shader;

struct Program
{
	unsigned int id = 0;

	Program();
	~Program();

	bool AddShaders(std::vector<Shader*> shaders);
	void Use();
	void SetMatrices(glm::mat4 model, glm::mat4 viewProjection);
	void SetFloat(std::string id, float value);
};
