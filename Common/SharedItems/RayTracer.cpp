#include "Chunk.h"
#include "RayTracer.h"

std::optional<glm::ivec3> RayTracer::GetFirstBlock(const Chunk* const chunk, Ray ray)
{
	const int iterations{ static_cast<int>(ray.Length() / TRACER_STEP) };
	const glm::vec3 direction{ ray.Direction() };
	glm::vec3 check{ ray.m_start };
	for (int i = 0; i < iterations; i++)
	{
		check += direction * TRACER_STEP;
		glm::ivec3 gridCheck(roundf(check.x), roundf(check.y), roundf(check.z));
		if (chunk->At(gridCheck)->IsSolid())
		{
			return gridCheck;
		}
	}
	return std::nullopt;
}
