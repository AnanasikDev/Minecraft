#pragma once

#include "glm/vec3.hpp"

struct StructureData
{
	unsigned char m_generatorIndex{ 0 };
	bool m_isGenerated{ false };
	glm::ivec3 m_worldPosition{ 0, 0, 0 };
	bool m_isComplete{ false };

	explicit StructureData(unsigned char generator, glm::ivec3 worldPosition) : m_generatorIndex(generator), m_worldPosition(worldPosition)
	{

	}
};