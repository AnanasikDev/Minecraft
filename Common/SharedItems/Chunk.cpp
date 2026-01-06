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
	m_isDirty = false;
	m_isGenerating = false;
	m_isReadyForRender = false;

	m_id = SGUID++;
	m_blocks = std::make_unique<std::array<Block, XWIDTH * YHEIGHT * ZDEPTH>>();
	m_meshRenderer.UseMesh(&m_mesh);
	m_meshRenderer.UseRendererSystem(m_game->m_renderer.get());
	m_meshRenderer.m_transform.Translate(LocalToWorld(glm::ivec3(0, 0, 0)));
	m_meshRenderer.UseProgram(m_game->m_renderer->m_helper->m_mainProgram.get());

	m_waterMeshRenderer.UseMesh(&m_waterMesh);
	m_waterMeshRenderer.UseRendererSystem(m_game->m_renderer.get());
	m_waterMeshRenderer.m_transform.Translate(LocalToWorld(glm::ivec3(0, 0, 0)));
	m_waterMeshRenderer.UseProgram(m_game->m_renderer->m_helper->m_waterProgram.get());

	m_debugMeshRenderer.UseMesh(&Mesh<DebugVertex>::MESH_BOX);
	m_debugMeshRenderer.UseRendererSystem(m_game->m_renderer.get());
	m_debugMeshRenderer.UpdateBuffers();
	m_debugMeshRenderer.m_mode = RENDER_MODE::WIREFRAME_MODE;
	m_debugMeshRenderer.m_transform.Translate(glm::vec3(LocalToWorld(glm::ivec3(0))) + glm::vec3(XWIDTH, YHEIGHT, ZDEPTH) / 2.0f);
	m_debugMeshRenderer.m_transform.ScaleLocal(glm::vec3(XWIDTH, YHEIGHT, ZDEPTH));
	m_debugMeshRenderer.UseProgram(m_game->m_renderer->m_helper->m_mainProgram.get());

	m_compositeMesh = std::make_unique<ChunkCompositeMesh<FVertex>>();
	m_compositeMesh->SetMesh(&m_mesh);
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

	if (data->IsWater())
	{
		m_compositeMesh->SetMesh(&m_waterMesh);
	}
	else
	{
		m_compositeMesh->SetMesh(&m_mesh);
	}

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
	if (World::IsLocalBlockInsideChunk(pos)) return AtForce(pos);
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

void Chunk::NewBlock(glm::ivec3 pos, Block::ID id, bool update)
{
	Block* const block{ AtSafe(pos) };
	if (!block || block->m_id == id) return;
	const glm::ivec3 worldpos{ LocalToWorld(pos) };
	const unsigned char prevEmission{ block->GetEmission() };
	const unsigned char lightLevel{ block->GetLightLevel() };

	if (prevEmission > 0)
	{
		auto it = std::find(m_lightSources.begin(), m_lightSources.end(), pos);
		if (it != m_lightSources.end())
		{
			m_lightSources.erase(it);
		}
		m_game->m_world->m_lightManager.PopLightSource(LightSource(worldpos, prevEmission));
	}
	block->Set(id, false);
	unsigned char newEmission{ block->GetEmission() };
	if (newEmission > 0)
	{
		m_lightSources.push_back(pos);
		m_game->m_world->m_lightManager.PushLightSource(LightSource(worldpos, newEmission));
	}
	else if (update && prevEmission == 0)
	{
		const bool isObstruction{ BlockData::IsSolid(id) };
		unsigned char newLightLevel{ 0 };
		for (glm::ivec3 shift : World::closeXYZNeighbours)
		{
			Block* b = AtGlobal(worldpos + shift);
			if (b)
			{
				unsigned char bl{ b->GetLightLevel() };
				if (bl == 0) continue;
				bl = static_cast<unsigned char>(bl - 1);
				if (bl > newLightLevel)
				{
					newLightLevel = bl;
				}
			}
		}
		unsigned char newSkyExposure{ 0 };
		Block* b = AtGlobal(worldpos + glm::ivec3(0, 1, 0));
		if (b && b->GetSkyExposure() == 15)
		{
			newSkyExposure = 15;
		}
		else
		{
			for (glm::ivec3 shift : World::closeXYZNeighbours)
			{
				Block* b = AtGlobal(worldpos + shift);
				if (b)
				{
					unsigned char bl{ b->GetSkyExposure() };
					if (bl == 0) continue;
					bl = static_cast<unsigned char>(bl - 1);
					if (bl > newSkyExposure)
					{
						newSkyExposure = bl;
					}
				}
			}
		}
		m_game->m_world->m_lightManager.PushNonLight(NonLight(worldpos, isObstruction, newLightLevel, newSkyExposure));
	}
}

void Chunk::UpdateGPUBuffers()
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_meshRenderer.UpdateBuffers();
	m_waterMeshRenderer.UpdateBuffers();
	m_isGenerating = false;
	m_isReadyForRender = true;
}

void Chunk::RenderSolid(Camera* camera)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_meshRenderer.Render(camera);
}

void Chunk::RenderTransparent(Camera* camera)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_waterMeshRenderer.Render(camera);
}

void Chunk::RenderDebug(Camera* camera)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_debugMeshRenderer.Render(camera);
}