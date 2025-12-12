#include "Block.h"
#include "Ray.h"
#include "Chunk.h"

Ray::Ray(glm::vec3 start, glm::vec3 end) : m_start(start), m_end(end)
{
}

RaycastResult::RaycastResult(Block* block, glm::ivec3 worldBlockPos, glm::vec3 worldPos, Chunk* chunk, glm::ivec3 normal) : m_block(block), m_worldBlockPos(worldBlockPos), m_worldPos(worldPos), m_chunk(chunk), m_normal(normal)
{

}