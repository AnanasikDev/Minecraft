#pragma once

#include <glm/vec3.hpp>

struct LightSource
{
	glm::ivec3 m_worldPos;
	unsigned char m_emission{ 0 };

	LightSource(const glm::ivec3& worldPos, unsigned char emission)
		: m_worldPos(worldPos), m_emission(emission)
	{
	}
};

struct NonLight
{
	glm::ivec3 m_worldPos;
	bool m_isObstruction;
	unsigned char m_lightLevel;
	unsigned char m_skyExposure;

	NonLight(const glm::ivec3& m_worldPos, bool m_isObstruction, unsigned char m_lightLevel, unsigned char m_skyExposure)
		: m_worldPos(m_worldPos), m_isObstruction(m_isObstruction), m_lightLevel(m_lightLevel), m_skyExposure(m_skyExposure)
	{
	}
};