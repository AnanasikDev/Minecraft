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

	if (std::is_same_v<GLT, short>) return GL_SHORT;
	if (std::is_same_v<GLT, unsigned short>) return GL_UNSIGNED_SHORT;

	if (std::is_same_v<GLT, char>) return GL_BYTE;
	if (std::is_same_v<GLT, unsigned char>) return GL_UNSIGNED_BYTE;

	printf("Unknown GLenum in %s:%d", __FILE__, __LINE__);
	return GL_FLOAT;
}

constexpr bool IsGLEnumTypeInt(GLenum type)
{
	return 
		type == GL_BYTE || 
		type == GL_UNSIGNED_BYTE || 
		type == GL_SHORT || 
		type == GL_UNSIGNED_SHORT || 
		type == GL_INT || 
		type == GL_UNSIGNED_INT;
}

using Coord = short;

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