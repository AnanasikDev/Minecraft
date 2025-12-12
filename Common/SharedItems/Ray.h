#pragma once
#include "glm/vec3.hpp"
#include "glm/glm.hpp"


struct Ray
{
	glm::vec3 m_start;
	glm::vec3 m_end;

	Ray(glm::vec3 start, glm::vec3 end);

	inline float Length() const { return glm::length(m_end - m_start); }
	inline glm::vec3 Direction() const { return glm::normalize(m_end - m_start); }
};

struct Block;
struct Chunk;

struct RaycastResult
{
	Block* m_block{ nullptr };
	glm::ivec3 m_worldBlockPos;
	glm::vec3 m_worldPos;
	glm::ivec3 m_normal;
	Chunk* m_chunk{ nullptr };

	RaycastResult(Block* block, glm::ivec3 worldBlockPos, glm::vec3 worldPos, Chunk* chunk, glm::ivec3 normal);
};