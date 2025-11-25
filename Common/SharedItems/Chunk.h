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
	static constexpr int XWIDTH{ 16 };
	static constexpr int YHEIGHT{ 16 };
	static constexpr int ZDEPTH{ 16 };

	static constexpr int MAX_STONE_HEIGHT{ 8 };

	int m_id{ 0 };
	bool m_isDirty{ true };
	std::atomic<bool> m_isGPUDirty{ false };
	bool m_isReadyForRender{ false };
	glm::ivec3 m_position{ 0,0,0 };
	std::atomic<bool> m_isGenerating{ false };
	int m_version{ 0 };

	std::unique_ptr<std::array<Block, XWIDTH * YHEIGHT * ZDEPTH>> m_blocks;
	Block* At(glm::ivec3 pos);
	const Block* At(glm::ivec3 pos) const;
	void NewBlock(glm::ivec3 pos, BlockData* blockdata);

	void Generate();
	void UpdateGPUBuffers();
	Chunk() = delete;
	Chunk(glm::ivec3 pos, Game* game);
	~Chunk();

	void Render(Camera* camera) override;
	void RenderDebug(Camera* camera) override;
	constexpr bool IsBlockOnEdge(glm::ivec3 pos) const;
	constexpr bool IsBlockInside(glm::ivec3 pos) const;

	void AddTop(glm::ivec3 pos, TextureAtlas::TextureID texid);
	void AddBottom(glm::ivec3 pos, TextureAtlas::TextureID texid);
	void AddRight(glm::ivec3 pos, TextureAtlas::TextureID texid);
	void AddLeft(glm::ivec3 pos, TextureAtlas::TextureID texid);
	void AddFront(glm::ivec3 pos, TextureAtlas::TextureID texid);
	void AddBack(glm::ivec3 pos, TextureAtlas::TextureID texid);

	glm::ivec3 LocalToWorld(glm::ivec3 pos) const;
	glm::ivec3 WorldToLocal(glm::ivec3 pos) const;

private:
	Game* m_game{ nullptr };
	Mesh<FVertex> m_mesh;
	MeshRenderer<FVertex> m_meshRenderer;
	std::mutex m_mtx;
	static int SGUID;
	//MeshRenderer<DebugVertex> m_debugMeshRenderer;

	void GenerateGrid();
	void AddFace(FVertex v00, FVertex v10, FVertex v11, FVertex v01);
	void GenerateBlock(glm::ivec3 pos, const Block& block);

	friend class World;
};
