#pragma once

#include <vector>
#include <memory>
#include <array>
#include <atomic>
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
#include "StructureData.h"

class Camera;
class Game;

template <typename Vertex>
struct ChunkCompositeMesh;

class Chunk : public IRenderable
{
public:
	static constexpr int XWIDTH{ 16 };
	static constexpr int YHEIGHT{ 128 };
	static constexpr int ZDEPTH{ 16 };
	static constexpr glm::ivec3 SIZE{ XWIDTH, YHEIGHT, ZDEPTH };
	static constexpr glm::ivec3 HALFSIZE{ XWIDTH / 2, YHEIGHT / 2, ZDEPTH / 2 };

	static constexpr int GROUND_HEIGHT{ 60 };

	int m_id{ 0 };
	std::atomic<bool> m_isDirty{ true };
	std::atomic<bool> m_isReadyForRender{ false };
	glm::ivec3 m_position{ 0,0,0 };
	std::atomic<bool> m_isGenerating{ false };
	int m_version{ 0 };
	std::atomic<int> m_controls{ 0 };
	std::atomic<bool> m_isDiscarded{ false };
	std::atomic<bool> m_isReadable{ false };

	std::unique_ptr<std::array<Block, XWIDTH * YHEIGHT * ZDEPTH>> m_blocks;
	Block* AtForce(glm::ivec3 pos);
	Block* AtSafe(glm::ivec3 pos);
	const Block* AtSafe(glm::ivec3 pos) const;
	const Block* AtForce(glm::ivec3 pos) const;
	Block* AtGlobal(glm::ivec3 pos);
	const Block* AtGlobal(glm::ivec3 pos) const;
	void NewBlock(glm::ivec3 pos, Block::ID id);

	void UpdateGPUBuffers();
	Chunk() = delete;
	Chunk(glm::ivec3 pos, Game* game);
	~Chunk();

	void Render(Camera* camera) override;
	void RenderDebug(Camera* camera) override;
	static std::array<glm::ivec3, 8> GetCorners(glm::ivec3 chunkGridPos);
	
	std::vector<glm::ivec3> GetChunkNeighboursAt(glm::ivec3 pos) const;

	/*void AddTop		(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next);
	void AddBottom	(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next);
	void AddRight	(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next);
	void AddLeft	(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next);
	void AddFront	(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next);
	void AddBack	(glm::ivec3 pos, TextureAtlas::TextureID texid, const Block* next);*/

	glm::ivec3 LocalToWorld(glm::ivec3 pos) const;
	glm::ivec3 WorldToLocal(glm::ivec3 pos) const;
	std::mutex m_mtx;

	StructureData* AddStructure(StructureData data);
	std::vector<StructureData>& GetStructures();

private:
	static int SGUID;
	Game* m_game{ nullptr };
	Mesh<FVertex> m_mesh;
	MeshRenderer<FVertex> m_meshRenderer;
	MeshRenderer<DebugVertex> m_debugMeshRenderer;
	std::vector<StructureData> m_structures;

	std::unique_ptr<ChunkCompositeMesh<FVertex>> m_compositeMesh;

	void GenerateBlock(glm::ivec3 pos, const Block& block, RemeshRequest* request);

	friend class World;
};
