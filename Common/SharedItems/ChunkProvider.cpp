#include "Chunk.h"
#include "World.h"
#include "ChunkProvider.h"

Chunk* ChunkProvider::GetChunkAt(glm::ivec3 gridpos) const
{
    switch (m_type)
    {
        case ProviderType::World:    return m_world->GetChunkAt(gridpos);
        case ProviderType::Request:  return m_request->GetChunkAt(gridpos);
    }
    assert(false);
    return nullptr;
}

ChunkProvider ChunkProvider::AsWorld(World* world)
{
    return ChunkProvider{ProviderType::World, world, nullptr};
}

ChunkProvider ChunkProvider::AsRequest(RemeshRequest* request)
{
    return ChunkProvider{ ProviderType::Request, nullptr, request };
}

Block* ChunkProvider::GetBlockAtWorld(glm::ivec3 pos) const
{
    switch (m_type)
    {
    case ProviderType::World:    return m_world->GetBlockAtWorld(pos);
    case ProviderType::Request:  return m_request->GetBlockAtWorld(pos);
    }
    assert(false);
    return nullptr;
}
