#pragma once

#include <stdio.h>
#include <glh.h>
#include "glm/vec3.hpp"
#include <type_traits>

template <typename GLT>
constexpr GLenum GetGLEnumType() {
	if (std::is_same_v<GLT, float>) return GL_FLOAT;
	if (std::is_same_v<GLT, int>) return GL_INT;
	if (std::is_same_v<GLT, unsigned int>) return GL_UNSIGNED_INT;
	printf("Unknown GLenum in %s:%d", __FILE__, __LINE__);
	return GL_FLOAT;
}

using Coord = short;

enum class ScalarDirection : char
{
	Negative = -1,
	Zero = 0,
	Positive = 1
};

enum class GridVec : unsigned char
{
	Invalid,
	Top,
	Bottom,
	Front,
	Back,
	Right,
	Left
};

glm::ivec3 GridVecToVec3(GridVec vec);

#define UnwrapGridVec(vec) vec.x, vec.y, vec.y