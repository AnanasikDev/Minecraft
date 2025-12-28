#pragma once

#include <queue>
#include <set>
#include <vector>
#include <functional>
#include <glm/vec3.hpp>
#include "Block.h"
#include "LightSource.h"
#include "ChunkProvider.h"

class World;

class LightManager
{
	struct BFSIter
	{
		Block* block;
		const BlockData* blockdata;
		glm::ivec3 globalpos;
		glm::ivec3 shift;
		unsigned char& currentvalue;
		unsigned char& targetvalue;

		BFSIter(Block* block, const BlockData* blockdata, glm::ivec3 globalpos, glm::ivec3 shift, unsigned char& currentvalue, unsigned char& targetvalue)
			: block(block), blockdata(blockdata), globalpos(globalpos), shift(shift), currentvalue(currentvalue), targetvalue(targetvalue)
		{
		}
	};
	struct PullResult
	{
		std::vector<Chunk*> m_dirtyChunks;
	};

	enum class NeighbourMode : char
	{
		CloseXZ,
		AllXZ,
		CloseXYZ,
		AllXYZ
	};
	enum class IterAns : char
	{
		SpreadFurther,
		StopSpreading
	};

	using fqueue = std::vector<std::pair<glm::ivec3, unsigned char>>;
	using fdirtyChunks = std::set<Chunk*>;
	using fIterFunc = std::function<IterAns(BFSIter& inp)>;
	using fOnBFSFinish = std::function<void(fdirtyChunks& dirtyChunks)>;
	using fOnBFSInit = std::function<void(Block* block)>;

	static constexpr unsigned char MIN_LIGHT{ 0 };
	static constexpr unsigned char MAX_LIGHT{ 15 };
	static constexpr unsigned char MIN_SKY_LIGHT{ 0 };
	static constexpr unsigned char MAX_SKY_LIGHT{ 15 };

	static constexpr int BFS_QUEUE_CAPACITY{ 4000 };
	static constexpr int BFS_SKY_QUEUE_CAPACITY{ 300 };

public:
	void Init(World* world);
	void PushLightSource(LightSource src);
	void PopLightSource(LightSource src);
	void PushNonLight(NonLight obstruction);
	void PushSkyLightSource(LightSource src);
	void EndFrame();

	void GenerateSkyExposure(RemeshRequest& request);

private:
	void PropagateLight(LightSource src, ChunkProvider chunkProvider, char step = -1);
	void PropagateSkyLight(LightSource src, ChunkProvider chunkProvider, char step = -1, bool update = false);
	void PropagateSkyDarkness(LightSource src, ChunkProvider chunkProvider, char step = -1);
	void PropagateDarkness(LightSource src, ChunkProvider chunkProvider, char step = -1);

	void BaseBFS(fqueue& queue, LightSource src, char step, NeighbourMode mode, bool markDirty, ChunkProvider chunkProvider, fIterFunc iterate, std::optional<fOnBFSInit> init = std::nullopt, std::optional<fOnBFSFinish> finish = std::nullopt);

	void RegenerateEmitters(fdirtyChunks& dirtyChunks, LightSource src);
	PullResult PullSkyLight(ChunkProvider chunkProvider, Chunk* chunk, std::queue<LightSource>& toAddSky);

	World* m_world{ nullptr };
	std::queue<NonLight> m_toSolid;
	std::queue<LightSource> m_toAdd;
	std::queue<LightSource> m_toRemove;
	std::queue<LightSource> m_toAddSky;
};