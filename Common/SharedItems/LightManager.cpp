#include <vector>
#include <set>
#include "World.h"
#include "LightManager.h"

void LightManager::Init(World* world)
{
	m_world = world;
}

void LightManager::PushLightSource(LightSource src)
{
	m_toAdd.push(src);
}

void LightManager::PopLightSource(LightSource src)
{
	m_toRemove.push(src);
}

void LightManager::PushNonLight(NonLight nonlight)
{
	m_toSolid.push(nonlight);
}

void LightManager::PushSkyLightSource(LightSource src)
{
	m_toAddSky.push(src);
}

void LightManager::EndFrame()
{
	while (!m_toSolid.empty())
	{
		NonLight block = m_toSolid.front();
		m_toSolid.pop();
		if (block.m_isObstruction)
		{
			PropagateDarkness(LightSource(block.m_worldPos, block.m_lightLevel), ChunkProvider::AsWorld(m_world));
			PropagateSkyDarkness(LightSource(block.m_worldPos, block.m_skyExposure), ChunkProvider::AsWorld(m_world));

			if (block.m_skyExposure == MAX_SKY_LIGHT)
			{
				glm::ivec3 worldpos = block.m_worldPos + glm::ivec3(0, -1, 0);
				PropagateSkyDarkness(LightSource(worldpos, MAX_SKY_LIGHT), ChunkProvider::AsWorld(m_world));
			}
		}
		else
		{
			PropagateLight(LightSource(block.m_worldPos, block.m_lightLevel), ChunkProvider::AsWorld(m_world));

			if (block.m_skyExposure == 15) // new sun beam
			{
				glm::ivec3 worldpos = block.m_worldPos + glm::ivec3(0, 1, 0);
				PropagateSkyLight(LightSource(worldpos, MAX_SKY_LIGHT), ChunkProvider::AsWorld(m_world), -1, true);
			}
			else
			{
				PropagateSkyLight(LightSource(block.m_worldPos, block.m_skyExposure), ChunkProvider::AsWorld(m_world), -1, true);
			}
		}
	}

	while (!m_toRemove.empty())
	{
		LightSource light = m_toRemove.front();
		m_toRemove.pop();
		light.m_emission = MAX_LIGHT;
		PropagateDarkness(light, ChunkProvider::AsWorld(m_world));
	}
	while (!m_toAdd.empty())
	{
		LightSource light = m_toAdd.front();
		m_toAdd.pop();
		PropagateLight(light, ChunkProvider::AsWorld(m_world));
	}

	while (!m_toAddSky.empty())
	{
		LightSource light = m_toAddSky.front();
		m_toAddSky.pop();
		Block* b = m_world->GetBlockAtWorld(light.m_worldPos);
		if (!b) continue;

		if (b->GetSkyExposure() < light.m_emission)
		{
			continue;
		}
		PropagateSkyLight(light, ChunkProvider::AsWorld(m_world), -1, true);
	}
}

void LightManager::GenerateSkyExposure(RemeshRequest& request)
{
	for (int x = 0; x < Chunk::XWIDTH; x++)
	{
		for (int z = 0; z < Chunk::ZDEPTH; z++)
		{
			for (int y = Chunk::YHEIGHT - 1; y >= 0; y--)
			{
				glm::ivec3 local(x, y, z);
				Block* const block = request.chunk->AtForce(local);
				if (BlockData::IsAir(block->GetID()))
				{
					block->SetSkyExposure(MAX_SKY_LIGHT);
				}
				else
				{
					break;
				}
			}
		}
	}

	glm::ivec3 worldpos = World::ChunkGridToWorldBlock(request.chunk->m_position);
	for (int x = 0; x < Chunk::XWIDTH; x++)
	{
		for (int z = 0; z < Chunk::ZDEPTH; z++)
		{
			glm::ivec3 local(x, Chunk::YHEIGHT - 1, z);
			Block* const block = request.chunk->AtForce(local);
			if (BlockData::IsAir(block->GetID()))
			{
				PropagateSkyLight(LightSource(worldpos + local, MAX_SKY_LIGHT), ChunkProvider::AsRequest(&request));
			}
			else
			{
				break;
			}
		}
	}

	std::queue<LightSource> toAddSky;
	PullResult pullResult = PullSkyLight(ChunkProvider::AsRequest(&request), request.chunk, toAddSky);
	if (!toAddSky.empty())
	{
		std::vector<std::pair<glm::ivec3, unsigned char>> queue;
		queue.reserve(2000);
		LightSource src = toAddSky.front();
		while (!toAddSky.empty())
		{
			LightSource light = toAddSky.front();
			toAddSky.pop();
			queue.emplace_back(light.m_worldPos, light.m_emission);
			//PropagateSkyLight(light, ChunkProvider::AsRequest(&request));
		}
		BaseBFS(queue, src, -1, NeighbourMode::CloseXZ, false, ChunkProvider::AsRequest(&request), [](BFSIter& it)
			{
				if (it.blockdata->IsLightable() && it.block->GetSkyExposure() < it.targetvalue)
				{
					it.block->SetSkyExposure(it.targetvalue);
					return IterAns::SpreadFurther;
				}
				return IterAns::StopSpreading;
			},
			[src](Block* block)
			{
				block->SetSkyExposure(src.m_emission);
			});
	}
}

void LightManager::PropagateLight(LightSource src, ChunkProvider chunkProvider, char step)
{
	if (src.m_emission == 0) return;

	static std::vector<std::pair<glm::ivec3, unsigned char>> queue;
	queue.reserve(BFS_QUEUE_CAPACITY);
	BaseBFS(queue, src, step, NeighbourMode::CloseXYZ, true, chunkProvider, [](BFSIter& it)
		{
			if (it.blockdata->IsLightable() && it.block->GetLightLevel() < it.targetvalue)
			{
				it.block->SetLightLevel(it.targetvalue);
				return IterAns::SpreadFurther;
			}
			return IterAns::StopSpreading;
		},
		[src](Block* block)
		{
			block->SetLightLevel(src.m_emission);
		},
		[](fdirtyChunks& dirtyChunks)
		{
			for (auto chunk : dirtyChunks)
			{
				chunk->m_isCustom = true;
			}
		});
}

void LightManager::PropagateDarkness(LightSource src, ChunkProvider chunkProvider, char step)
{
	static std::vector<std::pair<glm::ivec3, unsigned char>> queue;
	queue.reserve(BFS_QUEUE_CAPACITY);
	BaseBFS(queue, src, step, NeighbourMode::CloseXYZ, true, chunkProvider, [&, step](BFSIter& it)
		{
			unsigned char nextLight{ std::max(it.block->GetLightLevel(), it.block->GetEmission()) };
			if (it.blockdata->IsLightable() && nextLight != MIN_LIGHT)
			{
				if (nextLight < it.currentvalue)
				{
					it.block->SetLightLevel(MIN_LIGHT);
					return IterAns::SpreadFurther;
				}
				else
				{
					PushLightSource(LightSource(it.globalpos, nextLight));
				}
			}
			return IterAns::StopSpreading;
		},
		[src](Block* block)
		{
			block->SetLightLevel(MIN_LIGHT);
		},
		[&, src](fdirtyChunks& dirtyChunks)
		{
			RegenerateEmitters(dirtyChunks, src);
		});
}

void LightManager::PropagateSkyLight(LightSource src, ChunkProvider chunkProvider, char step, bool update)
{
	std::vector<std::pair<glm::ivec3, unsigned char>> queue;
	queue.reserve(BFS_SKY_QUEUE_CAPACITY);
	std::optional<fOnBFSFinish> onFinish = std::nullopt;
	if (update)
	{
		onFinish =
			[&](fdirtyChunks& dirtyChunks)
			{
				for (auto& c : dirtyChunks)
				{
					c->m_isDirty = true;
				}
			};
	}

	if (src.m_emission == MAX_SKY_LIGHT) // as sun beam
	{
		BaseBFS(queue, src, step, NeighbourMode::CloseXYZ, false, chunkProvider, [src, step](BFSIter& it)
			{
				unsigned char targetValue = it.targetvalue;
				bool beam = false;
				if (it.globalpos.x == src.m_worldPos.x && it.globalpos.z == src.m_worldPos.z && it.shift == glm::ivec3(0, -1, 0))
				{
					if (it.blockdata->IsTransparent() && !it.blockdata->IsAir() && targetValue >= -step)
					{
						targetValue += step;
					}
					else
					{
						targetValue = MAX_SKY_LIGHT;
						beam = true;
					}
				}
				it.targetvalue = targetValue;
				if (it.blockdata->IsLightable() && (it.block->GetSkyExposure() < targetValue || beam))
				{
					it.block->SetSkyExposure(targetValue);
					return IterAns::SpreadFurther;
				}
				return IterAns::StopSpreading;
			},
			[src](Block* block)
			{
				block->SetSkyExposure(src.m_emission);
			},
			onFinish);
	}
	else // as ambient light
	{
		BaseBFS(queue, src, step, NeighbourMode::CloseXYZ, false, chunkProvider, [src](BFSIter& it)
			{
				if (it.blockdata->IsLightable() && it.block->GetSkyExposure() < it.targetvalue)
				{
					it.block->SetSkyExposure(it.targetvalue);
					return IterAns::SpreadFurther;
				}
				return IterAns::StopSpreading;
			},
			[src](Block* block)
			{
				block->SetSkyExposure(src.m_emission);
			},
			onFinish);
	}
}

void LightManager::PropagateSkyDarkness(LightSource src, ChunkProvider chunkProvider, char step)
{
	std::vector<std::pair<glm::ivec3, unsigned char>> queue;
	queue.reserve(BFS_SKY_QUEUE_CAPACITY);
	if (src.m_emission == MAX_SKY_LIGHT) // as sun beam
	{
		BaseBFS(queue, src, step, NeighbourMode::CloseXYZ, false, chunkProvider, [&, src](BFSIter& it)
			{
				unsigned char targetValue = it.targetvalue;
				bool beam = false;
				if (it.currentvalue == MAX_SKY_LIGHT && it.shift.y == -1)
				{
					targetValue = MAX_SKY_LIGHT;
					beam = true;
				}
				it.targetvalue = targetValue;
				unsigned char nextLight = it.block->GetSkyExposure();
				if (it.blockdata->IsLightable() && nextLight != MIN_SKY_LIGHT)
				{
					if (nextLight < it.currentvalue || beam || (nextLight < MAX_SKY_LIGHT && nextLight == it.currentvalue))
					{
						it.block->SetSkyExposure(MIN_SKY_LIGHT);
						return IterAns::SpreadFurther;
					}
					else
					{
						PushSkyLightSource(LightSource(it.globalpos, nextLight));
					}
				}
				return IterAns::StopSpreading;
			},
			[src](Block* block)
			{
				block->SetSkyExposure(MIN_SKY_LIGHT);
			},
			[&](fdirtyChunks& dirtyChunks)
			{
				for (auto& c : dirtyChunks)
				{
					c->m_isDirty = true;
				}
			});
	}
	else
	{
		BaseBFS(queue, src, step, NeighbourMode::CloseXYZ, false, chunkProvider, [&, src](BFSIter& it)
			{
				unsigned char nextLight = it.block->GetSkyExposure();
				if (it.blockdata->IsLightable() && nextLight != MIN_SKY_LIGHT)
				{
					if (nextLight < it.currentvalue)
					{
						it.block->SetSkyExposure(MIN_SKY_LIGHT);
						return IterAns::SpreadFurther;
					}
					else
					{
						PushSkyLightSource(LightSource(it.globalpos, nextLight));
					}
				}
				return IterAns::StopSpreading;
			},
			[src](Block* block)
			{
				block->SetSkyExposure(MIN_SKY_LIGHT);
			},
			[&](fdirtyChunks& dirtyChunks)
			{
				for (auto& c : dirtyChunks)
				{
					c->m_isDirty = true;
				}
			});
	}
}

void LightManager::BaseBFS(fqueue& queue, LightSource src, char step, NeighbourMode mode, bool markDirty, ChunkProvider chunkProvider, fIterFunc iterate, std::optional<fOnBFSInit> init, std::optional<fOnBFSFinish> finish)
{
	queue.clear();
	size_t head{ 0 };

	Block* source = chunkProvider.GetBlockAtWorld(src.m_worldPos);
	assert(source);
	if (!source) return;

	if (init.has_value())
	{
		init.value()(source);
	}

	queue.push_back({ src.m_worldPos, src.m_emission });
	glm::ivec3 origChunkGridPos{ World::WorldBlockToChunkGrid(src.m_worldPos) };
	Chunk* origChunk{ chunkProvider.GetChunkAt(origChunkGridPos) };
	Chunk* localChunk{ nullptr };

	std::set<Chunk*> dirtyChunks;
	dirtyChunks.insert(origChunk);

	Block::ID blockid{ Block::ID::Stone };
	BlockData* blockdata{ BlocksDatabase::Get(blockid) };
	Block* b{ nullptr };
	glm::ivec3 nextpos;
	glm::ivec3 nextChunkGridPos;
	glm::ivec3 local;

	size_t length{ 0 };
	const glm::ivec3* shifts{ nullptr };
	switch (mode)
	{
	case NeighbourMode::AllXYZ:		length = 26; shifts = World::allXYZNeighbours; break;
	case NeighbourMode::CloseXYZ:	length = 6;  shifts = World::closeXYZNeighbours; break;
	case NeighbourMode::AllXZ:		length = 8;  shifts = World::allXZNeighbours; break;
	case NeighbourMode::CloseXZ:	length = 4;  shifts = World::closeXZNeighbours; break;
	}

	while (head < queue.size())
	{
		auto [pos, currentLight] = queue[head++];

		if (currentLight <= -step) continue;

		for (size_t i = 0; i < length; i++)
		{
			glm::ivec3 shift = shifts[i];
			nextpos = pos + shift;
			nextChunkGridPos = World::WorldBlockToChunkGrid(nextpos);
			local = World::WorldToLocalAny(nextpos);
			if (nextChunkGridPos == origChunkGridPos)
			{
				b = origChunk->AtForce(local);
			}
			else
			{
				if (!localChunk || localChunk->m_position != nextChunkGridPos)
				{
					localChunk = chunkProvider.GetChunkAt(nextChunkGridPos);
					if (!localChunk) continue;
					dirtyChunks.insert(localChunk);
				}
				b = localChunk->AtForce(local);
			}
			if (!b) continue;

			if (blockid != b->GetID())
			{
				blockid = b->GetID();
				blockdata = b->GetData();
			}

			unsigned char targetLight{ static_cast<unsigned char>(currentLight + step) };
			BFSIter inp(b, blockdata, nextpos, shift, currentLight, targetLight);
			IterAns ans{ iterate(inp) };
			if (ans == IterAns::SpreadFurther)
			{
				if (inp.targetvalue <= MAX_LIGHT)
				{
					queue.push_back(std::make_pair(nextpos, inp.targetvalue));
				}
			}
		}
	}

	if (finish.has_value())
	{
		finish.value()(dirtyChunks);
	}

	if (markDirty)
	{
		for (auto& dirtyChunk : dirtyChunks)
		{
			dirtyChunk->m_isDirty = true;
		}
	}
}

void LightManager::RegenerateEmitters(fdirtyChunks& dirtyChunks, LightSource src)
{
	glm::ivec3 origChunkGridPos{ m_world->WorldBlockToChunkGrid(src.m_worldPos) };
	Chunk* origChunk{ m_world->GetChunkAt(origChunkGridPos) };
	Chunk* localChunk{ nullptr };

	Block::ID blockid{ Block::ID::Stone };
	Block* b{ nullptr };
	glm::ivec3 nextpos;
	glm::ivec3 nextChunkGridPos;
	glm::ivec3 local;

	for (glm::ivec3 shift : World::closeXYZNeighbours)
	{
		nextpos = src.m_worldPos + shift;
		nextChunkGridPos = m_world->WorldBlockToChunkGrid(nextpos);
		local = m_world->WorldToLocalAny(nextpos);
		if (nextChunkGridPos == origChunkGridPos)
		{
			b = origChunk->AtForce(local);
		}
		else
		{
			if (!localChunk || localChunk->m_position != nextChunkGridPos)
			{
				localChunk = m_world->GetChunkAt(nextChunkGridPos);
				if (!localChunk) continue;
				dirtyChunks.insert(localChunk);
			}
			b = localChunk->AtForce(local);
		}
		if (!b) continue;

		if (blockid != b->GetID())
		{
			blockid = b->GetID();
		}
		if (b->IsLightEmitter())	
		{
			PushLightSource(LightSource(nextpos, b->GetEmission()));
		}
	}
}

LightManager::PullResult LightManager::PullSkyLight(ChunkProvider chunkProvider, Chunk* chunk, std::queue<LightSource>& toAddSky)
{
	const glm::ivec3 chunkPos{ chunk->m_position };

	static constexpr int minx[4]{ 0,					0,				Chunk::XWIDTH - 1,	0 };
	static constexpr int maxx[4]{ Chunk::XWIDTH,		Chunk::XWIDTH,	Chunk::XWIDTH,		1 };
	static constexpr int minz[4]{ Chunk::ZDEPTH - 1,	0,				0,					0 };
	static constexpr int maxz[4]{ Chunk::ZDEPTH,		1,				Chunk::ZDEPTH,		Chunk::ZDEPTH };
	static constexpr int size[4]{ Chunk::ZDEPTH,		Chunk::ZDEPTH,	Chunk::XWIDTH,		Chunk::XWIDTH};

	glm::ivec3 shift;
	glm::ivec3 neighToOrig;
	glm::ivec3 origLocal;
	glm::ivec3 neighLocal;

	Chunk* neighbourChunk{ nullptr };
	PullResult result;

	for (int side = 0; side < 4; side++)
	{
		shift = World::closeXZNeighbours[side];
		neighToOrig = shift * -(size[side] - 1); // mirror pos to orig chunk
		neighbourChunk = chunkProvider.GetChunkAt(chunk->m_position + shift);
		if (!neighbourChunk) continue;

		for (int x = minx[side]; x < maxx[side]; x++)
		{
			for (int z = minz[side]; z < maxz[side]; z++)
			{
				for (int y = 0; y < Chunk::YHEIGHT; y++)
				{
					neighLocal = glm::ivec3(x, y, z);
					origLocal = neighLocal + neighToOrig;
					Block* neighBlock = neighbourChunk->AtForce(neighLocal);
					Block* origBlock = chunk->AtForce(origLocal);
					unsigned char intensity{ neighBlock->GetSkyExposure() };
					if (intensity > 0 && intensity > origBlock->GetSkyExposure() + 1)
					{
						toAddSky.push(LightSource(chunk->LocalToWorld(origLocal), intensity - 1));
					}
				}
			}
		}
		result.m_dirtyChunks.push_back(neighbourChunk);
	}

	return result;
}
