#include "Chunk.h"
#include "Camera.h"
#include "glh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Game.h"
#include "Program.h"
#include "Shader.h"
#include "Random.h"
#include "Block.h"
#include "World.h"

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
	Generate();

	/*m_debugMeshRenderer.UseMesh(&Mesh<DebugVertex>::MESH_BOX);
	m_debugMeshRenderer.UseRendererSystem(m_game->m_renderer.get());
	m_debugMeshRenderer.UpdateBuffers();
	m_debugMeshRenderer.m_mode = RENDER_MODE::WIREFRAME_MODE;
	m_debugMeshRenderer.m_transform.Translate(glm::vec3(pos.x, 0, pos.y));
	m_debugMeshRenderer.m_transform.Scale(glm::vec3(16, 20, 16));*/
}

Chunk::~Chunk()
{
}

void Chunk::GenerateGrid()
{
	for (int x = 0; x < XWIDTH; x++)
	{
		for (int z = 0; z < ZDEPTH; z++)
		{
			for (int y = 0; y < YHEIGHT; y++)
			{
				glm::ivec3 locPos(x, y, z);
				glm::ivec3 worldPos = LocalToWorld(locPos);
				Block::ID id = m_game->m_world->GetBlockIDAt(worldPos);
				NewBlock(locPos, m_game->blocks->Get(id));
			}
		}
	}
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

void Chunk::GenerateBlock(glm::ivec3 pos, const Block& block)
{
	block.m_data->GenerateGeometry(Context(pos, GridVec::Right, this));
	block.m_data->GenerateGeometry(Context(pos, GridVec::Left, this));
	block.m_data->GenerateGeometry(Context(pos, GridVec::Top, this));
	block.m_data->GenerateGeometry(Context(pos, GridVec::Bottom, this));
	block.m_data->GenerateGeometry(Context(pos, GridVec::Front, this));
	block.m_data->GenerateGeometry(Context(pos, GridVec::Back, this));
}

constexpr bool Chunk::IsBlockOnEdge(glm::ivec3 pos) const
{
	return (pos.y == YHEIGHT - 1) || (pos.y == 0) || (pos.x == XWIDTH - 1) || (pos.x == 0) || (pos.z == ZDEPTH - 1) || (pos.z == 0);
}

constexpr bool Chunk::IsBlockInside(glm::ivec3 pos) const
{
	return (pos.y < YHEIGHT) && (pos.y >= 0) && (pos.x < XWIDTH) && (pos.x >= 0) && (pos.z < ZDEPTH) && (pos.z >= 0);
}

Block* Chunk::At(glm::ivec3 pos)
{
	if (IsBlockInside(pos)) return &(*m_blocks)[pos.z * XWIDTH * YHEIGHT + pos.x * YHEIGHT + pos.y];
	else return nullptr;
}

const Block* Chunk::At(glm::ivec3 pos) const
{
	if (IsBlockInside(pos)) return &(*m_blocks)[pos.z * XWIDTH * YHEIGHT + pos.x * YHEIGHT + pos.y];
	else return nullptr;
}

void Chunk::NewBlock(glm::ivec3 pos, BlockData* blockdata)
{
	Block* block = At(pos);
	if (!block || block->m_data == blockdata) return;
	
	block->Set(blockdata);
	m_isDirty = true;
	m_isReadyForRender = false;
}

void Chunk::Generate()
{
	GenerateGrid();
	m_meshRenderer.m_transform.Translate(LocalToWorld(glm::ivec3(0, 0, 0)));
}

void Chunk::UpdateGPUBuffers()
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_meshRenderer.UpdateBuffers();
	m_glSyncFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	m_isGenerating = false;
	//glFlush();
	//m_isReadyForRender = true;
}

void Chunk::Render(Camera* camera)
{
	while (glGetError());
	std::lock_guard<std::mutex> lock(m_mtx);
	if (m_glSyncFence != 0)
	{
		// Check the status of the fence without waiting indefinitely
		GLenum wait_ret = glClientWaitSync(m_glSyncFence, GL_SYNC_FLUSH_COMMANDS_BIT, 20000);

		if (wait_ret == GL_TIMEOUT_EXPIRED)
		{
			// GPU is still busy uploading the mesh. Do NOT draw.
			return;
		}

		// The fence has been signaled (GPU finished upload).
		glDeleteSync(m_glSyncFence);
		m_glSyncFence = 0; // Mark the fence as consumed
		//m_isReadyForRender = true;
	}
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error: " << err << std::endl;
	}

	//if (!m_isReadyForRender) return;

	m_meshRenderer.Render(camera);
}

void Chunk::RenderDebug(Camera* camera)
{
	//m_debugMeshRenderer.Render(camera);
}

void Chunk::AddFace(FVertex v00, FVertex v10, FVertex v11, FVertex v01)
{
	faces += 1;
	const int index{ m_mesh.GetVerticesCount() };
	FVertex v[4]{ v00, v10, v11, v01 };
	m_mesh.AddVertices(v, 4);
	unsigned int e[6]{ index + 3, index + 2, index, index + 2, index + 1, index };
	m_mesh.AddIndices(e, 6);
}

void Chunk::AddTop(glm::ivec3 pos, TextureAtlas::TextureID texid)
{
	FVertex v00(pos.x, pos.y + 1, pos.z, 0, 0, texid);
	FVertex v10(pos.x + 1, pos.y + 1, pos.z, 1, 0, texid);
	FVertex v11(pos.x + 1, pos.y + 1, pos.z + 1, 1, 1, texid);
	FVertex v01(pos.x, pos.y + 1, pos.z + 1, 0, 1, texid);
	AddFace(v00, v10, v11, v01);
}

void Chunk::AddBottom(glm::ivec3 pos, TextureAtlas::TextureID texid)
{
	FVertex v00(pos.x, pos.y, pos.z, 0, 0, texid);
	FVertex v10(pos.x + 1, pos.y, pos.z, 1, 0, texid);
	FVertex v11(pos.x + 1, pos.y, pos.z + 1, 1, 1, texid);
	FVertex v01(pos.x, pos.y, pos.z + 1, 0, 1, texid);
	AddFace(v01, v11, v10, v00);
}

void Chunk::AddRight(glm::ivec3 pos, TextureAtlas::TextureID texid)
{
	FVertex v00(pos.x + 1, pos.y + 1, pos.z, 1, 0, texid);
	FVertex v10(pos.x + 1, pos.y, pos.z, 1, 1, texid);
	FVertex v11(pos.x + 1, pos.y, pos.z + 1, 0, 1, texid);
	FVertex v01(pos.x + 1, pos.y + 1, pos.z + 1, 0, 0, texid);
	AddFace(v00, v10, v11, v01);
}

void Chunk::AddLeft(glm::ivec3 pos, TextureAtlas::TextureID texid)
{
	FVertex v00(pos.x, pos.y + 1, pos.z, 1, 0, texid);
	FVertex v10(pos.x, pos.y, pos.z, 1, 1, texid);
	FVertex v11(pos.x, pos.y, pos.z + 1, 0, 1, texid);
	FVertex v01(pos.x, pos.y + 1, pos.z + 1, 0, 0, texid);
	AddFace(v01, v11, v10, v00);
}

void Chunk::AddBack(glm::ivec3 pos, TextureAtlas::TextureID texid)
{
	FVertex v00(pos.x, pos.y, pos.z, 1, 1, texid);
	FVertex v10(pos.x + 1, pos.y, pos.z, 0, 1, texid);
	FVertex v11(pos.x + 1, pos.y + 1, pos.z, 0, 0, texid);
	FVertex v01(pos.x, pos.y + 1, pos.z, 1, 0, texid);
	AddFace(v00, v10, v11, v01);
}

void Chunk::AddFront(glm::ivec3 pos, TextureAtlas::TextureID texid)
{
	FVertex v00(pos.x, pos.y, pos.z + 1, 1, 1, texid);
	FVertex v10(pos.x + 1, pos.y, pos.z + 1, 0, 1, texid);
	FVertex v11(pos.x + 1, pos.y + 1, pos.z + 1, 0, 0, texid);
	FVertex v01(pos.x, pos.y + 1, pos.z + 1, 1, 0, texid);
	AddFace(v01, v11, v10, v00);
}