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

int Chunk::SUID = 0;

//Chunk::Chunk()
//{
//	id = SUID++;
//}

Chunk::Chunk(glm::ivec2 pos, Game* game) : m_position(pos), m_game(game)
{
	id = SUID++;
	m_blocks = std::make_unique<std::array<Block, XWIDTH * YHEIGHT * ZDEPTH>>();
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
			for (Coord y = 0; y < YHEIGHT; y++)
			{
				glm::ivec3 worldPos = LocalToWorld(x, y, z);
				//if (y > static_cast<int>(m_game->world->GetHeightAt(x, y, z) * YHEIGHT)) continue;
				if (y < static_cast<int>(m_game->m_world->GetHeightAt(worldPos.x, worldPos.y, worldPos.z) * YHEIGHT) && m_game->m_world->GetDensityAt(worldPos.x, worldPos.y, worldPos.z) < 0.4f)
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
	for (Coord x = 0; x < XWIDTH; x++)
	{
		for (Coord z = 0; z < ZDEPTH; z++)
		{
			for (Coord y = 0; y < YHEIGHT; y++)
			{
				Block* block = At(x, y, z);
				if (!block || block->IsAir()) continue;

				GenerateBlock(x, y, z, *block);
			}
		}
	}
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
	if (block)
	{
		block->Set(blockdata);
	}
}

void Chunk::Generate()
{
	GenerateGrid();
	GenerateGeometry();

	glGenVertexArrays(1, &m_vaoid);
	glBindVertexArray(m_vaoid);

	m_vbo.Bind();
	m_vbo.LinkExternalVertexBuffer(m_vertices.data(), m_vertices.size() * sizeof(FVertex), GL_STATIC_DRAW);
	m_vbo.ReadAtlas(m_game->atlas.m_id);

	m_ebo.Bind();
	m_ebo.LinkExternalElementBuffer(reinterpret_cast<unsigned int*>(m_indecies.data()), m_indecies.size() * sizeof(unsigned int), GL_STATIC_DRAW);

	m_vbo.Layout().PushAttribute<float>(3, FVertex::stride);			// position
	m_vbo.Layout().PushAttribute<float>(2, FVertex::stride);			// uv
	m_vbo.Layout().PushAttribute<int>(1, FVertex::stride);				// texture id
}

void Chunk::Render(Camera* camera)
{
	if (id == 0) return;

	glBindVertexArray(m_vaoid);

	glm::mat4 matModel(1.0f);
	/*matModel = glm::translate(matModel, glm::vec3(0.5f, 0.0f, 5.0f));
	matModel = glm::rotate(matModel, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));*/
	glm::mat4 matProjection = camera->GetProjection();
	glm::mat4 matView = camera->GetView();

	unsigned int umodel = glGetUniformLocation(m_game->program->shaderProgram, "u_ModelMat");
	glUniformMatrix4fv(umodel, 1, GL_FALSE, glm::value_ptr(matModel));
	unsigned int uview = glGetUniformLocation(m_game->program->shaderProgram, "u_ViewMat");
	glUniformMatrix4fv(uview, 1, GL_FALSE, glm::value_ptr(matView));
	unsigned int uproj = glGetUniformLocation(m_game->program->shaderProgram, "u_ProjMat");
	glUniformMatrix4fv(uproj, 1, GL_FALSE, glm::value_ptr(matProjection));

	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, m_indecies.size(), GL_UNSIGNED_INT, nullptr);
}

void Chunk::AddFace(FVertex v00, FVertex v10, FVertex v11, FVertex v01)
{
	int index = m_vertices.size();
	v00.Add(m_position.x * XWIDTH, 0, m_position.y * ZDEPTH);
	v10.Add(m_position.x * XWIDTH, 0, m_position.y * ZDEPTH);
	v11.Add(m_position.x * XWIDTH, 0, m_position.y * ZDEPTH);
	v01.Add(m_position.x * XWIDTH, 0, m_position.y * ZDEPTH);
	m_vertices.push_back(v00);
	m_vertices.push_back(v10);
	m_vertices.push_back(v11);
	m_vertices.push_back(v01);

	m_indecies.push_back(index + 3);
	m_indecies.push_back(index + 2);
	m_indecies.push_back(index);
	m_indecies.push_back(index + 2);
	m_indecies.push_back(index + 1);
	m_indecies.push_back(index);
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