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
			const glm::vec3 fpos{ AMath::CastFloat(pos) };
			switch (data.biom)
			{
			case Biom::Forest:
			{
				if (Random::GetFloat2D(fpos.x * 10, fpos.z * 10) < 0.98f)
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
			default:
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
				glm::ivec3 localpos(
					Random::GetInt2D(chunkGridPos.x * 11 + i, chunkGridPos.z * 5 + i, 0, 15),
					0,
					Random::GetInt2D(chunkGridPos.x * 2 + i, chunkGridPos.z * 13 + i, 0, 15)
				);
				glm::ivec3 worldpos = localpos + World::ChunkGridToWorldBlock(chunkGridPos);
				BlockGenData data = GetBlockGenDataAt(worldpos);
				worldpos.y = data.GetHeight() + 1;
				data = GetBlockGenDataAt(worldpos);
				if (data.IsCave() || data.isWater)
				{
					continue;
				}
				result.push_back(worldpos);
			}
			return result;
		}
	);
}

void WorldGen::GenerateChunkGrid(RemeshRequest& request)
{
	Chunk* const chunk{ request.chunk };
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
	for (const glm::ivec3& shift : World::allXZNeighbours)
	{
		RegenerateStructures(chunk->m_position + shift);
	}
	chunk->m_isReadable = true;
}

BlockGenData WorldGen::GetBlockGenDataAt(glm::ivec3 pos)
{
	auto eps = [](float t)
	{
		float n{ 1.0f - powf(t / 1.1f, 4.0f) };
		return n;
	};

	glm::vec3 fpos3{ AMath::CastFloat(pos) };
	glm::vec2 fpos2{ glm::vec2(fpos3.x, fpos3.z)};
	constexpr float humShiftX = 1234.0f, humShiftY = 90001.0f;

	float temperatureNoise{ Noise2D_Positive(fpos2, glm::vec2(TEMPERATURE_NOISE_SCALE)) };
	float humidityNoise{ Noise2D_Positive(fpos2, glm::vec2(HUMIDITY_NOISE_SCALE), glm::vec2(humShiftX, humShiftY)) };
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

	float baseNoise{ Noise2D_Normalized(fpos2, glm::vec2(BASE_NOISE_SCALE)) };
	float base{ Chunk::GROUND_HEIGHT + (0.2f + baseNoise) * 5.5f };
	float mountainsNoise{ powf(Noise2D_Positive(fpos2, glm::vec2(MOUNTAINS_NOISE_SCALE)), 2.5f) };
	float mountains{ mountainsNoise * (Chunk::YHEIGHT - Chunk::GROUND_HEIGHT) };
	mountains *= eps(temperature);
	base *= eps(temperature);
	float height{ base + mountains };
	float caveness
	{
		powf(Noise3D_Normalized(fpos3, glm::vec3(CAVES_NOISE_SCALE)), 3.0f) +
		powf(Noise3D_Normalized(fpos3, glm::vec3(CAVES_NOISE_SCALE * 0.2f)), 4.0f) * 0.775f
	};

	bool isWater{ fpos3.y > height && fpos3.y < SEA_LEVEL };
	if (fpos3.y > height - BEACH_LENGTH && fpos3.y < SEA_LEVEL + BEACH_LENGTH)
	{
		biom = Biom::Beach;
	}

	return BlockGenData{ height, caveness, baseNoise, mountainsNoise, temperature, humidity, biom, isWater };
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

	if (data.isWater)
	{
		return Block::ID::Water;
	}

	if (worldPos.y > data.GetHeight() || data.IsCave())
	{
		return Block::ID::Air;
	}

	if (worldPos.y < data.GetHeight() - 5 || data.noiseMountains > 0.85f)
	{
		return Block::ID::Stone;
	}

	if (worldPos.y <= data.GetHeight())
	{
		switch (data.biom)
		{

			case Biom::Desert:
			{
				return Block::ID::Sand;
			} break;

			case Biom::Taiga:
			case Biom::Forest:
			{
				if (worldPos.y == data.GetHeight())
				{
					return Block::ID::Grass;
				}
				else
				{
					return Block::ID::Dirt;
				}
			} break;

			case Biom::Beach:
			{
				return Block::ID::Sand;
			} break;

		}
	}

	return Block::ID::Air;
}


void WorldGen::GenerateStructures(Chunk* chunk)
{
	assert(m_structureGenerators.size() <= 0xFF);
	for (size_t structureIndex = 0; structureIndex < m_structureGenerators.size(); structureIndex++)
	{
		std::vector<glm::ivec3> positions = m_structureGenerators[structureIndex].GetPositions(chunk->m_position);
		for (size_t posIndex = 0; posIndex < positions.size(); posIndex++)
		{
			StructureData temp(static_cast<unsigned char>(structureIndex), positions[posIndex]);
			StructureData* const data{ chunk->AddStructure(temp) };
			m_structureGenerators[structureIndex].Generate(positions[posIndex], *data, false);
		}
	}
}

void WorldGen::RegenerateStructures(glm::ivec3 chunkGridPos)
{
	assert(m_structureGenerators.size() <= 0xFF);
	for (size_t structureIndex = 0; structureIndex < m_structureGenerators.size(); structureIndex++)
	{
		std::vector<glm::ivec3> positions = m_structureGenerators[structureIndex].GetPositions(chunkGridPos);
		for (size_t posIndex = 0; posIndex < positions.size(); posIndex++)
		{
			StructureData data(static_cast<unsigned char>(structureIndex), positions[posIndex]);
			m_structureGenerators[structureIndex].Generate(positions[posIndex], data, true);
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
	const glm::vec3 fpos(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));

	constexpr int loweradd    { 2 };
	constexpr int upperadd	  { 1 };
	constexpr int lowerheight { 2 };
	constexpr int trunkheight { 5 };

	for (int x = -loweradd; x <= loweradd; x++)
	{
		for (int z = -loweradd; z <= loweradd; z++)
		{
			for (int y = trunkheight - 3; y < trunkheight - 3 + lowerheight; y++)
			{
				const glm::ivec3 block{ pos + glm::ivec3(x, y, z) };
				TrySetStructureBlock(block, Block::ID::OakLeaves, structure, chunkGridPos, regenerate);
			}
		}
	}

	for (int x = -upperadd; x <= upperadd; x++)
	{
		for (int z = -upperadd; z <= upperadd; z++)
		{
			for (int y = trunkheight - 1; y < trunkheight + 1; y++)
			{
				if ((abs(x) != abs(z) || (x == 0 && z == 0)) || Random::GetFloat2D(fpos.x + static_cast<float>(x), fpos.z + static_cast<float>(z)) > 0.8f)
				{
					const glm::ivec3 block{ pos + glm::ivec3(x, y, z) };
					TrySetStructureBlock(block, Block::ID::OakLeaves, structure, chunkGridPos, regenerate);
				}
			}
		}
	}
	for (int i = 0; i < trunkheight; i++)
	{
		const glm::ivec3 block{ pos + glm::ivec3(0, i, 0) };
		TrySetStructureBlock(block, Block::ID::OakLog, structure, chunkGridPos, regenerate);
	}
}

static float sdCone(glm::vec3 p, glm::vec2 c, float h)
{
	// from https://iquilezles.org/articles/distfunctions/
	// c is the sin/cos of the angle, h is height
	// Alternatively pass q instead of (c,h),
	// which is the point at the base in 2D
	const glm::vec2 q = h * glm::vec2(c.x / c.y, -1.0f);
	const glm::vec2 w = glm::vec2(glm::length(glm::vec2(p.x, p.z)), p.y);
	const glm::vec2 a = w - q * glm::clamp(dot(w, q) / dot(q, q), 0.0f, 1.0f);
	const glm::vec2 b = w - q * glm::vec2(glm::clamp(w.x / q.x, 0.0f, 1.0f), 1.0f);
	const float k = glm::sign(q.y);
	const float d = glm::min(dot(a, a), dot(b, b));
	const float s = glm::max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
	return sqrtf(d) * glm::sign(s);
}

void WorldGen::GenerateTree_Fir(glm::ivec3 pos, StructureData& structure, glm::ivec3 chunkGridPos, const BlockGenData* data, bool regenerate)
{
	constexpr int loweradd		{ 2 };
	constexpr int trunkheight	{ 5 };
	constexpr float angle{ glm::radians(18.0f) };
	const float sina { sinf(angle) };
	const float cosa { cosf(angle) };
	constexpr int height{ 14 };

	for (int x = -loweradd; x <= loweradd; x++)
	{
		for (int z = -loweradd; z <= loweradd; z++)
		{
			for (int y = 0; y < height; y++)
			{
				if (sdCone(glm::vec3(x, y - height, z), glm::vec2(sina, cosa), height) < 0)
				{
					const glm::ivec3 block{ pos + glm::ivec3(x, 3 + y, z) };
					TrySetStructureBlock(block, Block::ID::FirLeaves, structure, chunkGridPos, regenerate);

				}
			}
		}
	}
	for (int i = 0; i < trunkheight; i++)
	{
		const glm::ivec3 block{ pos + glm::ivec3(0, i, 0) };
		TrySetStructureBlock(block, Block::ID::FirLog, structure, chunkGridPos, regenerate);
	}
}

float WorldGen::Noise2D_Normalized(glm::vec2 pos, glm::vec2 scale, glm::vec2 shift)
{
	const float x = pos.x * scale.x + shift.x;
	const float y = pos.y * scale.y + shift.y;

	return static_cast<float>(m_perlin.noise2D(static_cast<double>(x), static_cast<double>(y)));
}

float WorldGen::Noise2D_Positive(glm::vec2 pos, glm::vec2 scale, glm::vec2 shift)
{
	const float n = Noise2D_Normalized(pos, scale, shift);
	return AMath::Remap(n, -1.0f, 1.0f, 0.0f, 1.0f);
}

float WorldGen::Noise3D_Normalized(glm::vec3 pos, glm::vec3 scale, glm::vec3 shift)
{
	const float x = pos.x * scale.x + shift.x;
	const float y = pos.y * scale.y + shift.y;
	const float z = pos.z * scale.z + shift.z;

	return static_cast<float>(m_perlin.noise3D(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)));
}

float WorldGen::Noise3D_Positive(glm::vec3 pos, glm::vec3 scale, glm::vec3 shift)
{
	const float n = Noise3D_Normalized(pos, scale, shift);
	return AMath::Remap(n, -1.0f, 1.0f, 0.0f, 1.0f);
}

