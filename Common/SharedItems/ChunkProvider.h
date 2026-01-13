#pragma once

#include <glm/vec3.hpp>

enum class ProviderType { World, Request };
class World;
struct RemeshRequest;
class Chunk;
struct Block;

struct ChunkProvider
{
    ProviderType m_type;
    World* m_world = nullptr;
    RemeshRequest* m_request = nullptr;

    Chunk* GetChunkAt(glm::ivec3 gridpos) const;
    static ChunkProvider AsWorld(World* world);
    static ChunkProvider AsRequest(RemeshRequest* request);

    Block* GetBlockAtWorld(glm::ivec3 pos) const;
};
