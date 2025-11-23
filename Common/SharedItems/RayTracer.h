#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/norm.hpp>
#include <optional>

struct Ray
{
	glm::vec3 m_start;
	glm::vec3 m_end;

	Ray(glm::vec3 start, glm::vec3 end) : m_start(start), m_end(end)
	{
	}

	inline float Length() const { return glm::length(m_end - m_start); }
	inline glm::vec3 Direction() const { return glm::normalize(m_end - m_start); }
};

class Chunk;

class RayTracer
{
public:
	std::optional<glm::ivec3> GetFirstBlock(const Chunk* const chunk, Ray ray);

private:
	static constexpr float TRACER_STEP = 0.1f;
};