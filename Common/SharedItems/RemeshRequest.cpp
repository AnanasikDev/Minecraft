#include "Chunk.h"
#include "RemeshRequest.h"
#include "World.h"

void RemeshRequest::SetControl()
{
	if (chunk)	chunk->m_controls++;
	if (neighbourNorth)	neighbourNorth->m_controls++;
	if (neighbourSouth)	neighbourSouth->m_controls++;
	if (neighbourUp)	neighbourUp->m_controls++;
	if (neighbourDown)	neighbourDown->m_controls++;
	if (neighbourEast)	neighbourEast->m_controls++;
	if (neighbourWest)	neighbourWest->m_controls++;
}

void RemeshRequest::ResetControl()
{
	if (chunk)	chunk->m_controls--;
	if (neighbourNorth)	neighbourNorth->m_controls--;
	if (neighbourSouth)	neighbourSouth->m_controls--;
	if (neighbourUp)	neighbourUp->m_controls--;
	if (neighbourDown)	neighbourDown->m_controls--;
	if (neighbourEast)	neighbourEast->m_controls--;
	if (neighbourWest)	neighbourWest->m_controls--;
}

Chunk*& RemeshRequest::Iterate(size_t index)
{
	assert(index >= 0 && index < CAPACITY);
	switch (index)
	{
		// + main chunk
	case 0: return neighbourNorth;
	case 1: return neighbourSouth;
	case 2: return neighbourUp;
	case 3: return neighbourDown;
	case 4: return neighbourEast;
	case 5: return neighbourWest;
	}
	return neighbourNorth;
}

Chunk* RemeshRequest::GetChunkAt(glm::ivec3 gridpos)
{
	if (chunk			&& gridpos == chunk->m_position)	return chunk;
	if (neighbourNorth	&& gridpos == neighbourNorth->m_position)	return neighbourNorth;
	if (neighbourSouth	&& gridpos == neighbourSouth->m_position)	return neighbourSouth;
	if (neighbourEast	&& gridpos == neighbourEast->m_position)	return neighbourEast;
	if (neighbourWest	&& gridpos == neighbourWest->m_position)	return neighbourWest;
	if (neighbourUp		&& gridpos == neighbourUp->m_position)		return neighbourUp;
	if (neighbourDown	&& gridpos == neighbourDown->m_position)	return neighbourDown;

	return nullptr;
}

Block* RemeshRequest::GetBlockAtWorld(glm::ivec3 pos)
{
	glm::ivec3 chunkGridPos{ World::WorldBlockToChunkGrid(pos) };
	Chunk* chunk{ GetChunkAt(chunkGridPos) };
	if (!chunk) return nullptr;
	return chunk->AtForce(World::WorldToLocalAny(pos));
}

bool RemeshRequest::AreAllValidXZ()
{
	return neighbourNorth && neighbourSouth && neighbourEast && neighbourWest;
}

bool RemeshRequest::AreAllGeneratedXZ()
{
	return AreAllValidXZ() && neighbourNorth->m_version > 0 && neighbourSouth->m_version > 0 && neighbourEast->m_version > 0 && neighbourWest->m_version > 0;
}

//std::unordered_map<glm::ivec3, Chunk*> RemeshRequest::GetMap()
//{
//	std::unordered_map<glm::ivec3, Chunk*> map;
//	if (neighbourNorth)	map.emplace(neighbourNorth->m_position, neighbourNorth);
//	if (neighbourSouth)	map.emplace(neighbourSouth->m_position, neighbourSouth);
//	if (neighbourUp)	map.emplace(neighbourUp->m_position, neighbourUp);
//	if (neighbourDown)	map.emplace(neighbourDown->m_position, neighbourDown);
//	if (neighbourEast)	map.emplace(neighbourEast->m_position, neighbourEast);
//	if (neighbourWest)	map.emplace(neighbourWest->m_position, neighbourWest);
//	return map;
//}
