#pragma once

#include <functional>
#include "glm/vec3.hpp"

struct BlockGenData;

struct StructureGenerator
{
	enum class Model
	{
		Tree
	};

	Model m_model;
	std::function<void(glm::ivec3, StructureData& structure, bool regenerate)> m_Generate;
	std::function<std::vector<glm::ivec3>(glm::ivec3 chunkGridPos)> m_GetPositions;

	inline void Generate(glm::ivec3 pos, StructureData& structure, bool regenerate) const
	{
		m_Generate(pos, structure, regenerate);
	}
	inline std::vector<glm::ivec3> GetPositions(glm::ivec3 chunkGridPos)
	{
		return m_GetPositions(chunkGridPos);
	}

	StructureGenerator(const Model& m_model, const std::function<void(glm::ivec3, StructureData& structure, bool regenerate)>& m_Generate, const std::function<std::vector<glm::ivec3>(glm::ivec3 chunkGridPos)>& m_GetPositions)
		: m_model(m_model), m_Generate(m_Generate), m_GetPositions(m_GetPositions)
	{
	}
};
