#include "Chunk.h"
#include "Camera.h"
#include "glh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "amath.h"
#include "Game.h"
#include "Program.h"
#include "Shader.h"
#include "Random.h"
#include "Block.h"
#include "World.h"
#include "RendererHelper.h"
#include "ChunkCompositeMesh.h"

int Chunk::SGUID = 0;

Chunk::Chunk(glm::ivec3 pos, Game* game) : m_position(pos), m_game(game)
{
	m_isDirty = true;
	m_isGenerating = false;
	m_isReadyForRender = false;

	m_id = SGUID++;
	m_blocks = std::make_unique<std::array<Block, XWIDTH * YHEIGHT * ZDEPTH>>();
	m_meshRenderer.UseMesh(&m_mesh);
	m_meshRenderer.UseRendererSystem(m_game->m_renderer.get());
	m_meshRenderer.m_transform.Translate(LocalToWorld(glm::ivec3(0, 0, 0)));
	//Generate();

	m_debugMeshRenderer.UseMesh(&Mesh<DebugVertex>::MESH_BOX);
	m_debugMeshRenderer.UseRendererSystem(m_game->m_renderer.get());
	m_debugMeshRenderer.UpdateBuffers();
	m_debugMeshRenderer.m_mode = RENDER_MODE::WIREFRAME_MODE;
	m_debugMeshRenderer.m_transform.Translate(glm::vec3(LocalToWorld(glm::ivec3(0))) + glm::vec3(XWIDTH, YHEIGHT, ZDEPTH) / 2.0f);
	m_debugMeshRenderer.m_transform.ScaleLocal(glm::vec3(XWIDTH, YHEIGHT, ZDEPTH));

	m_compositeMesh = std::make_unique<ChunkCompositeMesh<FVertex>>();
	m_compositeMesh->m_meshPtr = &m_mesh;
}

Chunk::~Chunk()
{
}

glm::ivec3 Chunk::LocalToWorld(glm::ivec3 pos) const
{
	return glm::ivec3
	(
		m_position.x * XWIDTH + pos.x,
		pos.y,
		m_position.z * ZDEPTH + pos.z
	);
}

glm::ivec3 Chunk::WorldToLocal(glm::ivec3 pos) const
{
	return pos - LocalToWorld(glm::ivec3(0, 0, 0));
}

StructureData* Chunk::AddStructure(StructureData data)
{
	m_structures.push_back(data);
	return &m_structures.front();
}

std::vector<StructureData>& Chunk::GetStructures()
{
	return m_structures;
}

void Chunk::GenerateBlock(glm::ivec3 localPos, const Block& block, RemeshRequest* request)
{
	BlockData* data = BlocksDatabase::Get(block.m_id);
	glm::ivec3 worldPos{ LocalToWorld(localPos) };
	data->GenerateGeometry(GeomContext(localPos, worldPos, GridVec::Right,	 m_game->m_world.get(), request, m_compositeMesh.get()));
	data->GenerateGeometry(GeomContext(localPos, worldPos, GridVec::Left,	 m_game->m_world.get(), request, m_compositeMesh.get()));
	data->GenerateGeometry(GeomContext(localPos, worldPos, GridVec::Top,	 m_game->m_world.get(), request, m_compositeMesh.get()));
	data->GenerateGeometry(GeomContext(localPos, worldPos, GridVec::Bottom,  m_game->m_world.get(), request, m_compositeMesh.get()));
	data->GenerateGeometry(GeomContext(localPos, worldPos, GridVec::Front,	 m_game->m_world.get(), request, m_compositeMesh.get()));
	data->GenerateGeometry(GeomContext(localPos, worldPos, GridVec::Back,	 m_game->m_world.get(), request, m_compositeMesh.get()));
}

std::array<glm::ivec3, 8> Chunk::GetCorners(glm::ivec3 chunkGridPos)
{
	glm::ivec3 center = World::ChunkGridToWorldBlock(chunkGridPos);
	return {
		glm::ivec3(center.x, center.y - 0,      center.z),
		glm::ivec3(center.x, center.y - 0,      center.z + SIZE.z),
		glm::ivec3(center.x, center.y + SIZE.y, center.z),
		glm::ivec3(center.x, center.y + SIZE.y, center.z + SIZE.z),
		glm::ivec3(center.x + SIZE.x, center.y - 0,      center.z),
		glm::ivec3(center.x + SIZE.x, center.y - 0,      center.z + SIZE.z),
		glm::ivec3(center.x + SIZE.x, center.y + SIZE.y, center.z),
		glm::ivec3(center.x + SIZE.x, center.y + SIZE.y, center.z + SIZE.z),
	};
}

std::vector<glm::ivec3> Chunk::GetChunkNeighboursAt(glm::ivec3 pos) const
{
	std::vector<glm::ivec3> result;
	if (pos.x == 0) result.push_back(glm::ivec3(-1, 0, 0));
	if (pos.x == XWIDTH - 1) result.push_back(glm::ivec3(1, 0, 0));

	if (pos.y == 0) result.push_back(glm::ivec3(0, -1, 0));
	if (pos.y == YHEIGHT - 1) result.push_back(glm::ivec3(0, 1, 0));

	if (pos.z == 0) result.push_back(glm::ivec3(0, 0, -1));
	if (pos.z == ZDEPTH - 1) result.push_back(glm::ivec3(0, 0, 1));
	return result;
}

Block* Chunk::AtForce(glm::ivec3 pos)
{
	return &(*m_blocks)[pos.z * XWIDTH * YHEIGHT + pos.x * YHEIGHT + pos.y];
}

Block* Chunk::AtSafe(glm::ivec3 pos)
{
	if (World::IsLocalBlockInsideChunk(pos)) AtForce(pos);
	else return nullptr;
}

const Block* Chunk::AtSafe(glm::ivec3 pos) const
{
	if (World::IsLocalBlockInsideChunk(pos)) return AtForce(pos);
	else return nullptr;
}

const Block* Chunk::AtForce(glm::ivec3 pos) const
{
	return &(*m_blocks)[pos.z * XWIDTH * YHEIGHT + pos.x * YHEIGHT + pos.y];
}

Block* Chunk::AtGlobal(glm::ivec3 pos)
{
	return AtSafe(WorldToLocal(pos));
}

const Block* Chunk::AtGlobal(glm::ivec3 pos) const
{
	return AtSafe(WorldToLocal(pos));
}

void Chunk::NewBlock(glm::ivec3 pos, Block::ID id)
{
	Block* block = AtSafe(pos);
	if (!block || block->m_id == id) return;

	block->Set(id);
	if (BlockData::IsLightSource(id))
	{
		m_game->m_world->UpdateLightBlock(LocalToWorld(pos), block->GetLightLevel());
	}
}

void Chunk::UpdateGPUBuffers()
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_meshRenderer.UpdateBuffers();
	m_isGenerating = false;
	m_isReadyForRender = true;
}

void Chunk::Render(Camera* camera)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_meshRenderer.Render(camera);
}

void Chunk::RenderDebug(Camera* camera)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_debugMeshRenderer.Render(camera);
}

//void Chunk::AddTop(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next)
//{
//	VRendererHelper<FVertex>::AddFace(m_mesh, pos, GridVec::Top, texid, Block::GetLightLevelSafe(next));
//}
//
//void Chunk::AddBottom(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next)
//{
//	VRendererHelper<FVertex>::AddFace(m_mesh, pos, GridVec::Bottom, texid, Block::GetLightLevelSafe(next));
//}
//
//void Chunk::AddRight(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next)
//{
//	VRendererHelper<FVertex>::AddFace(m_mesh, pos, GridVec::Right, texid, Block::GetLightLevelSafe(next));
//}
//
//void Chunk::AddLeft(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next)
//{
//	VRendererHelper<FVertex>::AddFace(m_mesh, pos, GridVec::Left, texid, Block::GetLightLevelSafe(next));
//}
//
//void Chunk::AddBack(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next)
//{
//	VRendererHelper<FVertex>::AddFace(m_mesh, pos, GridVec::Back, texid, Block::GetLightLevelSafe(next));
//}
//
//void Chunk::AddFront(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next)
//{
//	VRendererHelper<FVertex>::AddFace(m_mesh, pos, GridVec::Front, texid, Block::GetLightLevelSafe(next));
//}