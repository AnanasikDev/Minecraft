#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>

#include "StructureGenerator.h"
#include "PerlinNoise.hpp"
#include "RemeshRequest.h"

enum class Biom
{
	Forest,
	Desert,
	Taiga
};

struct BlockGenData
{
	float columnHeight;
	float density;
	float noiseBase;
	float noiseMountains;

	float temperature;
	float humidity;
	Biom biom;

	inline int GetHeight() const
	{
		return static_cast<int>(columnHeight);
	}
};

class WorldGen
{
public:
	static constexpr float BASE_NOISE_SCALE{ 0.045f };
	static constexpr float TEMPERATURE_NOISE_SCALE{ 0.01f };
	static constexpr float HUMIDITY_NOISE_SCALE{ 0.02f };
	static constexpr float CAVES_NOISE_SCALE{ 0.075f };
	static constexpr float MOUNTAINS_NOISE_SCALE{ 0.01f };

	WorldGen(World* world);
	~WorldGen() = default;

	void GenerateChunkGrid(RemeshRequest& request);
	BlockGenData GetBlockGenDataAt(glm::ivec3 pos);
	Block::ID GetBlockIDAt(glm::ivec3 worldpos);
	Block::ID GetBlockIDAt(glm::ivec3 worldpos, BlockGenData& outdata);

	void GenerateStructures(Chunk* chunk);
	void RegenerateStructures(glm::ivec3 chunkGridPos);
	void TrySetStructureBlock(glm::ivec3 worldpos, Block::ID id, StructureData& structure, glm::ivec3 chunkGridPos, bool regenerate);
	void GenerateTree_Oak(glm::ivec3 pos, StructureData& structure, glm::ivec3 chunkGridPos, const BlockGenData* data, bool regenerate);
	void GenerateTree_Fir(glm::ivec3 pos, StructureData& structure, glm::ivec3 chunkGridPos, const BlockGenData* data, bool regenerate);

	static inline std::string GetBiomName(Biom biom)
	{
		switch (biom)
		{
		case Biom::Desert: return "Desert";
		case Biom::Forest: return "Forest";
		case Biom::Taiga: return "Taiga";
		default: return "UNKNOWN";
		}
	}

private:
	World* m_world{ nullptr };
	siv::PerlinNoise::seed_type m_seed{ 123456u };
	siv::PerlinNoise m_perlin{ m_seed };
	std::vector<StructureGenerator> m_structureGenerators;
};