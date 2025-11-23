#pragma once

#include <vector>
#include <memory>
#include <array>
#include <mutex>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "IRenderable.h"
#include "Vertex.h"
#include "Mesh.h"
#include "BlockData.h"
#include "Block.h"
#include "BlocksDatabase.h"
#include "commons.h"

class Camera;
class Game;

class Chunk : public IRenderable
{
public:
	static constexpr Coord XWIDTH{ 16 };
	static constexpr Coord YHEIGHT{ 16 };
	static constexpr Coord ZDEPTH{ 16 };

	static constexpr Coord MAX_STONE_HEIGHT{ 5 };

	int m_id{ 0 };
	bool m_isDirty{ true };
	bool m_isGPUDirty{ false };
	bool m_isReadyForRender{ false };
	glm::ivec2 m_position;
	std::atomic<int> m_workersAccess{ 0 };

	std::unique_ptr<std::array<Block, XWIDTH* YHEIGHT* ZDEPTH>> m_blocks;
	Block* At(Coord x, Coord y, Coord z);
	const Block* At(Coord x, Coord y, Coord z) const;
	void NewBlock(Coord x, Coord y, Coord z, BlockData* blockdata);

	void Generate();
	void Remesh();
	void UpdateGPUBuffers();
	Chunk() = delete;
	Chunk(glm::ivec2 pos, Game* game);
	~Chunk();

	void Render(Camera* camera) override;
	void RenderDebug(Camera* camera) override;
	constexpr bool IsBlockOnEdge(Coord x, Coord y, Coord z) const;
	constexpr bool IsBlockInside(Coord x, Coord y, Coord z) const;

	void AddTop(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddBottom(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddRight(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddLeft(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddFront(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);
	void AddBack(Coord x, Coord y, Coord z, TextureAtlas::TextureID texid);

private:
	Game* m_game{ nullptr };
	Mesh<FVertex> m_mesh;
	MeshRenderer<FVertex> m_meshRenderer;
	std::mutex m_mtx;
	static int SGUID;

	void GenerateGrid();
	void GenerateGeometry();
	glm::ivec3 LocalToWorld(Coord x, Coord y, Coord z) const;

	void AddFace(FVertex v00, FVertex v10, FVertex v11, FVertex v01);
	
	void GenerateBlock(Coord x, Coord y, Coord z, const Block& block);

	friend class World;
};
