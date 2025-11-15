#pragma once

#include <vector>
#include <memory>
#include <array>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "IRenderable.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "BlockData.h"
#include "Block.h"
#include "BlocksDatabase.h"
#include "commons.h"

class Camera;
class Game;

class Chunk : public IRenderable
{
public:
	static constexpr Coord XWIDTH = 16;
	static constexpr Coord YHEIGHT = 16;
	static constexpr Coord ZDEPTH = 16;

	static constexpr Coord MAX_STONE_HEIGHT = 5;

	int id;
	glm::ivec2 m_position;

	std::unique_ptr<std::array<Block, XWIDTH * YHEIGHT * ZDEPTH>> m_blocks;
	Block* At(Coord x, Coord y, Coord z);
	const Block* At(Coord x, Coord y, Coord z) const;
	void NewBlock(Coord x, Coord y, Coord z, BlockData* blockdata);

	void Generate();
	//Chunk();
	Chunk(glm::ivec2 pos, Game* game);
	~Chunk();
	
	void Render(Camera* camera) override;
	constexpr bool IsBlockOnEdge(Coord x, Coord y, Coord z) const;
	constexpr bool IsBlockInside(Coord x, Coord y, Coord z) const;

	void AddTop(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddBottom(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddRight(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddLeft(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddFront(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddBack(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);

private:
	Game* m_game;
	unsigned int m_vaoid;
	VertexBuffer m_vbo;
	ElementBuffer m_ebo;
	std::vector<FVertex> m_vertices;
	std::vector<unsigned int> m_indecies;
	static int SUID;

	void GenerateGrid();
	void GenerateGeometry();
	glm::ivec3 LocalToWorld(Coord x, Coord y, Coord z) const;

	void AddFace(FVertex v00, FVertex v10, FVertex v11, FVertex v01);
	
	void GenerateBlock(Coord x, Coord y, Coord z, const Block& block);
};
