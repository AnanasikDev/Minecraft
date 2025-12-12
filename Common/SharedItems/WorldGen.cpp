#include "World.h"
#include "WorldGen.h"
#include "Random.h"

WorldGen::WorldGen(World* world) : m_world(world)
{
	m_structureGenerators.emplace_back(
		StructureGenerator::Model::Tree,
		[&](glm::ivec3 pos, StructureData& structure, bool regenerate)
		{
			BlockGenData data = GetBlockGenDataAt(pos);
			glm::ivec3 chunkGridPos = world->WorldBlockToChunkGrid(pos);
			structure.m_isComplete = true;
			switch (data.biom)
			{
			case Biom::Forest:
			{
				if (Random::GetFloat2D(pos.x * 10, pos.z * 10) < 0.98f)
				{
					GenerateTree_Oak(pos, structure, chunkGridPos, &data, regenerate);
				}
				else
				{
					GenerateTree_Fir(pos, structure, chunkGridPos, &data, regenerate);
				}
			} break;
			case Biom::Taiga:
			{
				GenerateTree_Fir(pos, structure, chunkGridPos, &data, regenerate);
			} break;
			case Biom::Desert:
			{

			} break;
			}
			structure.m_isGenerated = true;
		},
		[&](glm::ivec3 chunkGridPos)
		{
			int num = Random::GetInt2D(chunkGridPos.x, chunkGridPos.z, 5, 7);
			std::vector<glm::ivec3> result;
			for (int i = 0; i < num; i++)
			{
				glm::ivec3 pos(
					Random::GetInt2D(chunkGridPos.x * 11 + i, chunkGridPos.z * 5 + i, 0, 15),
					0,
					Random::GetInt2D(chunkGridPos.x * 2 + i, chunkGridPos.z * 13 + i, 0, 15)
				);
				pos += World::ChunkGridToWorldBlock(chunkGridPos);
				BlockGenData data = GetBlockGenDataAt(pos);
				pos.y = static_cast<int>(data.columnHeight) + 1;
				result.push_back(pos);
			}
			return result;
		}
	);
}

void WorldGen::GenerateChunkGrid(RemeshRequest& request)
{
	Chunk* chunk = request.chunk;
	for (int x = 0; x < Chunk::XWIDTH; x++)
	{
		for (int z = 0; z < Chunk::ZDEPTH; z++)
		{
			for (int y = 0; y < Chunk::YHEIGHT; y++)
			{
				glm::ivec3 locPos(x, y, z);
				glm::ivec3 worldPos = chunk->LocalToWorld(locPos);
				BlockGenData data;
				Block::ID id = GetBlockIDAt(worldPos, data);
				chunk->NewBlock(locPos, id);
			}
		}
	}

	GenerateStructures(chunk);
	static constexpr glm::ivec3 shifts[8]{
		glm::ivec3(-1, 0, -1),
		glm::ivec3(-1, 0, 0),
		glm::ivec3(-1, 0, 1),
		glm::ivec3(0, 0, -1),
		glm::ivec3(0, 0, 1),
		glm::ivec3(1, 0, -1),
		glm::ivec3(1, 0, 0),
		glm::ivec3(1, 0, 1)
	};
	for (const glm::ivec3& shift : shifts)
	{
		RegenerateStructures(chunk->m_position + shift);
	}
	chunk->m_isReadable = true;
}

BlockGenData WorldGen::GetBlockGenDataAt(glm::ivec3 pos)
{
	std::function<float(float)> eps = [](float t)
	{
		float n = 1 - powf(t / 1.1f, 4.0f);
		return n;
	};

	float temperatureNoise{ AMath::Remap(static_cast<float>(m_perlin.noise2D(pos.x * TEMPERATURE_NOISE_SCALE, pos.z * TEMPERATURE_NOISE_SCALE)), -1.0f, 1.0f, 0.0f, 1.0f) };
	float humidityNoise{ AMath::Remap(static_cast<float>(m_perlin.noise2D(pos.x * HUMIDITY_NOISE_SCALE + 1234, pos.z * HUMIDITY_NOISE_SCALE - 90001)), -1.0f, 1.0f, 0.0f, 1.0f) };
	float temperature{ temperatureNoise };
	float humidity{ humidityNoise };
	Biom biom;

	if (temperature > 0.6f)
	{
		biom = Biom::Desert;
	}
	else if (temperature < 0.3f)
	{
		biom = Biom::Taiga;
	}
	else
	{
		biom = Biom::Forest;
	}

	float baseNoise{ static_cast<float>(m_perlin.noise2D(pos.x * BASE_NOISE_SCALE, pos.z * BASE_NOISE_SCALE)) };
	float base{ Chunk::GROUND_HEIGHT + (0.2f + baseNoise) * 5.5f };
	float mountainsNoise{ powf(AMath::Remap(static_cast<float>(m_perlin.noise2D(pos.x * MOUNTAINS_NOISE_SCALE, pos.z * MOUNTAINS_NOISE_SCALE)), -1.0f, 1.0f, 0.0f, 1.0f), 2.5f) };
	float mountains{ mountainsNoise * (Chunk::YHEIGHT - Chunk::GROUND_HEIGHT) };
	mountains *= eps(temperature);
	base *= eps(temperature);
	float height{ base + mountains };
	float density{ powf(static_cast<float>(m_perlin.noise3D
	(
		static_cast<double>(pos.x) * static_cast<double>(CAVES_NOISE_SCALE),
		static_cast<double>(pos.y) * static_cast<double>(CAVES_NOISE_SCALE),
		static_cast<double>(pos.z) * static_cast<double>(CAVES_NOISE_SCALE)
	)), 3.0f) };

	return BlockGenData{ height, density, baseNoise, mountainsNoise, temperature, humidity, biom };
}

Block::ID WorldGen::GetBlockIDAt(glm::ivec3 worldPos)
{
	BlockGenData data;
	return GetBlockIDAt(worldPos, data);
}

Block::ID WorldGen::GetBlockIDAt(glm::ivec3 worldPos, BlockGenData& outdata)
{
	const BlockGenData data{ GetBlockGenDataAt(worldPos) };
	outdata = data;

	if (worldPos.y == 0)
	{
		return Block::ID::Bedrock;
	}

	if (worldPos.y >= data.columnHeight || data.density > 0.1f)
	{
		return Block::ID::Air;
	}

	if (worldPos.y < data.columnHeight - 5 || data.noiseMountains > 0.85f)
	{
		return Block::ID::Stone;
	}

	if (worldPos.y < data.columnHeight)
	{
		switch (data.biom)
		{
		case Biom::Desert:
			return Block::ID::Sand;
		case Biom::Taiga:
		case Biom::Forest:
		{
			if (worldPos.y == static_cast<int>(data.columnHeight))
			{
				return Block::ID::Grass;
			}
			else
			{
				return Block::ID::Dirt;
			}
		}
		}
	}
}


void WorldGen::GenerateStructures(Chunk* chunk)
{
	for (int i = 0; i < m_structureGenerators.size(); i++)
	{
		std::vector<glm::ivec3> positions = m_structureGenerators[i].GetPositions(chunk->m_position);
		for (int s = 0; s < positions.size(); s++)
		{
			StructureData data(i, positions[s]);
			StructureData* d = chunk->AddStructure(data);
			m_structureGenerators[i].Generate(positions[s], *d, false);
		}
	}
}

void WorldGen::RegenerateStructures(glm::ivec3 chunkGridPos)
{
	for (int i = 0; i < m_structureGenerators.size(); i++)
	{
		std::vector<glm::ivec3> positions = m_structureGenerators[i].GetPositions(chunkGridPos);
		for (int s = 0; s < positions.size(); s++)
		{
			StructureData data(i, positions[s]);
			m_structureGenerators[i].Generate(positions[s], data, true);
		}
	}
}

void WorldGen::TrySetStructureBlock(glm::ivec3 worldpos, Block::ID id, StructureData& structure, glm::ivec3 chunkGridPos, bool regenerate)
{
	if (regenerate && World::IsWorldBlockInsideChunk(chunkGridPos, worldpos))
	{
		return;
	}
	if (!m_world->ForceSetBlockAtWorld(worldpos, id))
	{
		structure.m_isComplete = false;
	}
}

void WorldGen::GenerateTree_Oak(glm::ivec3 pos, StructureData& structure, glm::ivec3 chunkGridPos, const BlockGenData* data, bool regenerate)
{
	constexpr int loweradd = 2;
	constexpr int upperadd = 1;
	constexpr int lowerheight = 2;
	constexpr int upperheight = 2;
	constexpr int truckheight = 5;

	for (int x = -loweradd; x <= loweradd; x++)
	{
		for (int z = -loweradd; z <= loweradd; z++)
		{
			for (int y = truckheight - 3; y < truckheight - 3 + lowerheight; y++)
			{
				glm::ivec3 block = pos + glm::ivec3(x, y, z);
				TrySetStructureBlock(block, Block::ID::OakLeaves, structure, chunkGridPos, regenerate);
			}
		}
	}

	for (int x = -upperadd; x <= upperadd; x++)
	{
		for (int z = -upperadd; z <= upperadd; z++)
		{
			for (int y = truckheight - 1; y < truckheight + 1; y++)
			{
				if ((abs(x) != abs(z) || x == 0 && z == 0) || Random::GetFloat2D(pos.x + x, pos.z + z) > 0.8f)
				{
					glm::ivec3 block = pos + glm::ivec3(x, y, z);
					TrySetStructureBlock(block, Block::ID::OakLeaves, structure, chunkGridPos, regenerate);
				}
			}
		}
	}
	for (int i = 0; i < truckheight; i++)
	{
		glm::ivec3 block = pos + glm::ivec3(0, i, 0);
		TrySetStructureBlock(block, Block::ID::OakLog, structure, chunkGridPos, regenerate);
	}
}

static float sdCone(glm::vec3 p, glm::vec2 c, float h)
{
	// from https://iquilezles.org/articles/distfunctions/
	// c is the sin/cos of the angle, h is height
	// Alternatively pass q instead of (c,h),
	// which is the point at the base in 2D
	glm::vec2 q = h * glm::vec2(c.x / c.y, -1.0f);

	glm::vec2 w = glm::vec2(glm::length(glm::vec2(p.x, p.z)), p.y);
	glm::vec2 a = w - q * glm::clamp(dot(w, q) / dot(q, q), 0.0f, 1.0f);
	glm::vec2 b = w - q * glm::vec2(glm::clamp(w.x / q.x, 0.0f, 1.0f), 1.0f);
	float k = glm::sign(q.y);
	float d = glm::min(dot(a, a), dot(b, b));
	float s = glm::max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
	return sqrt(d) * glm::sign(s);
}

void WorldGen::GenerateTree_Fir(glm::ivec3 pos, StructureData& structure, glm::ivec3 chunkGridPos, const BlockGenData* data, bool regenerate)
{
	constexpr int loweradd = 2;
	constexpr int lowerheight = 2;
	constexpr int truckheight = 5;
	constexpr float angle = glm::radians(18.0f);
	float sina = sinf(angle);
	float cosa = cosf(angle);
	constexpr float height = 14;

	for (int x = -loweradd; x <= loweradd; x++)
	{
		for (int z = -loweradd; z <= loweradd; z++)
		{
			for (int y = 0; y < height; y++)
			{
				if (sdCone(glm::vec3(x, y - height, z), glm::vec2(sina, cosa), height) < 0)
				{
					glm::ivec3 block = pos + glm::ivec3(x, 3 + y, z);
					TrySetStructureBlock(block, Block::ID::FirLeaves, structure, chunkGridPos, regenerate);

				}
			}
		}
	}
	for (int i = 0; i < truckheight; i++)
	{
		glm::ivec3 block = pos + glm::ivec3(0, i, 0);
		TrySetStructureBlock(block, Block::ID::FirLog, structure, chunkGridPos, regenerate);
	}
}