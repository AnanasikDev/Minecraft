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

Chunk::Chunk(glm::ivec2 pos, Game* game) : m_position(pos), m_game(game), m_isDirty(true), m_isGPUDirty(true)
{
	m_id = SGUID++;
	m_blocks = std::make_unique<std::array<Block, XWIDTH * YHEIGHT * ZDEPTH>>();
	m_meshRenderer.UseMesh(&m_mesh);
	m_meshRenderer.UseRendererSystem(m_game->m_renderer.get());
	Generate();
}

Chunk::~Chunk()
{
}

void Chunk::GenerateGrid()
{
	for (Coord x = 0; x < XWIDTH; x++)
	{
		for (Coord z = 0; z < ZDEPTH; z++)
		{
			glm::ivec3 worldPos = LocalToWorld(x, 0, z);
			const float height = static_cast<int>(m_game->m_world->GetHeightAt(worldPos.x, worldPos.z) * YHEIGHT);

			for (Coord y = 0; y < YHEIGHT; y++)
			{
				worldPos = LocalToWorld(x, y, z);
				if (y < height && m_game->m_world->GetDensityAt(worldPos.x, worldPos.y, worldPos.z) < 0.4f)
				{
					if (Random::GetFloat(0.0f, std::fminf(static_cast<float>(y) / MAX_STONE_HEIGHT, MAX_STONE_HEIGHT) < 0.5f))
					{
						NewBlock(x, y, z, m_game->blocks->Get(Block::ID::Stone));
					}
					else{
						NewBlock(x, y, z, m_game->blocks->Get(Block::ID::Grass));
					}
				}
				else
				{
					NewBlock(x, y, z, m_game->blocks->Get(Block::ID::Air));
				}
			}
		}
	}
}

void Chunk::GenerateGeometry()
{
}

glm::ivec3 Chunk::LocalToWorld(Coord x, Coord y, Coord z) const
{
	return glm::ivec3(m_position.x * XWIDTH + x, y, m_position.y * ZDEPTH + z);
}

void Chunk::GenerateBlock(Coord x, Coord y, Coord z, const Block& block)
{
	block.m_data->GenerateGeometry(Context(x, y, z, GridVec::Right, this));
	block.m_data->GenerateGeometry(Context(x, y, z, GridVec::Left, this));
	block.m_data->GenerateGeometry(Context(x, y, z, GridVec::Top, this));
	block.m_data->GenerateGeometry(Context(x, y, z, GridVec::Bottom, this));
	block.m_data->GenerateGeometry(Context(x, y, z, GridVec::Front, this));
	block.m_data->GenerateGeometry(Context(x, y, z, GridVec::Back, this));
}

constexpr bool Chunk::IsBlockOnEdge(Coord x, Coord y, Coord z) const
{
	return (y == YHEIGHT - 1) || (y == 0) || (x == XWIDTH - 1) || (x == 0) || (z == ZDEPTH - 1) || (z == 0);
}

constexpr bool Chunk::IsBlockInside(Coord x, Coord y, Coord z) const
{
	return (y < YHEIGHT) && (y >= 0) && (x < XWIDTH) && (x >= 0) && (z < ZDEPTH) && (z >= 0);
}

Block* Chunk::At(Coord x, Coord y, Coord z)
{
	if (IsBlockInside(x, y, z)) return &(*m_blocks)[z * XWIDTH * YHEIGHT + x * YHEIGHT + y];
	else return nullptr;
}

const Block* Chunk::At(Coord x, Coord y, Coord z) const
{
	if (IsBlockInside(x, y, z)) return &(*m_blocks)[z * XWIDTH * YHEIGHT + x * YHEIGHT + y];
	else return nullptr;
}

void Chunk::NewBlock(Coord x, Coord y, Coord z, BlockData* blockdata)
{
	Block* block = At(x, y, z);
	if (!block || block->m_data == blockdata) return;
	
	block->Set(blockdata);
	m_isDirty = true;
	m_isGPUDirty = true;
	m_isReadyForRender = false;
}

void Chunk::Generate()
{
	GenerateGrid();
	m_meshRenderer.m_transform.Translate(LocalToWorld(0, 0, 0));
}

void Chunk::Remesh()
{
}

void Chunk::UpdateGPUBuffers()
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_meshRenderer.UpdateBuffers();
	m_isGPUDirty = false;
	m_isReadyForRender = true;
}

void Chunk::Render(Camera* camera)
{
	if (m_isDirty || m_isGPUDirty || !m_isReadyForRender) return;

	std::lock_guard<std::mutex> lock(m_mtx);
	
	if (m_isDirty || m_isGPUDirty || !m_isReadyForRender) return;

	m_meshRenderer.Render(camera);
}

void Chunk::RenderDebug(Camera* camera)
{
	/*return;
	static float vertices[8 * 3] = {
		0, 0, 0,
		0, 1, 0,
		1, 1, 0,
		1, 0, 0,
		0, 0, 1,
		0, 1, 1,
		1, 1, 1,
		1, 0, 1,
	};

	static unsigned int indices[12 * 2] = {
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	glBindVertexArray(m_debugvaoid);
	m_debugvbo.Bind();
	m_debugvbo.LinkExternalVertexBuffer(vertices, 8 * 3 * sizeof(float), GL_STATIC_DRAW);
	m_debugebo.Bind();
	m_debugebo.LinkExternalElementBuffer(indices, 12 * 2 * sizeof(unsigned int), GL_STATIC_DRAW);
	m_debugvbo.Layout().PushAttribute<float>(3, 3 * sizeof(float));

	glm::mat4 matModel(1.0f);
	matModel = glm::scale(matModel, glm::vec3(XWIDTH, YHEIGHT, ZDEPTH));
	matModel = glm::translate(matModel, glm::vec3(m_position.x * XWIDTH, 0, m_position.y * ZDEPTH));
	glm::mat4 matProjection = camera->GetProjection();
	glm::mat4 matView = camera->GetView();

	unsigned int umodel = glGetUniformLocation(m_game->program->shaderProgram, "u_ModelMat");
	glUniformMatrix4fv(umodel, 1, GL_FALSE, glm::value_ptr(matModel));
	unsigned int uview = glGetUniformLocation(m_game->program->shaderProgram, "u_ViewMat");
	glUniformMatrix4fv(uview, 1, GL_FALSE, glm::value_ptr(matView));
	unsigned int uproj = glGetUniformLocation(m_game->program->shaderProgram, "u_ProjMat");
	glUniformMatrix4fv(uproj, 1, GL_FALSE, glm::value_ptr(matProjection));

	glDrawElements(GL_LINES, 12 * 2, GL_UNSIGNED_INT, nullptr);*/
}

void Chunk::AddFace(FVertex v00, FVertex v10, FVertex v11, FVertex v01)
{
	const int index{ m_mesh.GetVerticesCount() };
	FVertex v[4]{ v00, v10, v11, v01 };
	m_mesh.AddVertices(v, 4);
	unsigned int e[6]{ index + 3, index + 2, index, index + 2, index + 1, index };
	m_mesh.AddIndices(e, 6);
}

void Chunk::AddTop(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid)
{
	FVertex v00(x, y + 1, z, 0, 0, texid);
	FVertex v10(x + 1, y + 1, z, 1, 0, texid);
	FVertex v11(x + 1, y + 1, z + 1, 1, 1, texid);
	FVertex v01(x, y + 1, z + 1, 0, 1, texid);
	AddFace(v00, v10, v11, v01);
}

void Chunk::AddBottom(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid)
{
	FVertex v00(x, y, z, 0, 0, texid);
	FVertex v10(x + 1, y, z, 1, 0, texid);
	FVertex v11(x + 1, y, z + 1, 1, 1, texid);
	FVertex v01(x, y, z + 1, 0, 1, texid);
	AddFace(v01, v11, v10, v00);
}

void Chunk::AddRight(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid)
{
	FVertex v00(x + 1, y + 1, z, 1, 0, texid);
	FVertex v10(x + 1, y, z, 1, 1, texid);
	FVertex v11(x + 1, y, z + 1, 0, 1, texid);
	FVertex v01(x + 1, y + 1, z + 1, 0, 0, texid);
	AddFace(v00, v10, v11, v01);
}

void Chunk::AddLeft(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid)
{
	FVertex v00(x, y + 1, z, 1, 0, texid);
	FVertex v10(x, y, z, 1, 1, texid);
	FVertex v11(x, y, z + 1, 0, 1, texid);
	FVertex v01(x, y + 1, z + 1, 0, 0, texid);
	AddFace(v01, v11, v10, v00);
}

void Chunk::AddBack(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid)
{
	FVertex v00(x, y, z, 1, 1, texid);
	FVertex v10(x + 1, y, z, 0, 1, texid);
	FVertex v11(x + 1, y + 1, z, 0, 0, texid);
	FVertex v01(x, y + 1, z, 1, 0, texid);
	AddFace(v00, v10, v11, v01);
}

void Chunk::AddFront(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid)
{
	FVertex v00(x, y, z + 1, 1, 1, texid);
	FVertex v10(x + 1, y, z + 1, 0, 1, texid);
	FVertex v11(x + 1, y + 1, z + 1, 0, 0, texid);
	FVertex v01(x, y + 1, z + 1, 1, 0, texid);
	AddFace(v01, v11, v10, v00);
}