#pragma once

#include "commons.h"
#include "CompositeMesh.h"
#include "Block.h"
#include "RendererHelper.h"
#include "BlockData.h"

struct Chunk;
struct RemeshRequest;

template <typename Vertex>
struct ChunkCompositeMesh : public BaseCompositeMesh<Vertex>
{
	/// <summary>
	/// Returns block instance at specific position, nullptr if outside the chunk.
	/// </summary>
	const Block* At(const GeomContext& ctx, GridVec vec) const
	{
		glm::ivec3 v = GridVecToVec3(vec);
		const Block* result = ctx.request->chunk->AtSafe(ctx.localPos + v);
		if (result) return result;
		Chunk* chunks[6]{ ctx.request->neighbourDown, ctx.request->neighbourUp, ctx.request->neighbourNorth, ctx.request->neighbourSouth, ctx.request->neighbourEast, ctx.request->neighbourWest };
		for (Chunk*& chunk : chunks)
		{
			if (chunk)
			{
				result = chunk->AtGlobal(ctx.globalPos + v);
				if (result) return result;
			}
		}
		return nullptr;
	}

	/// <summary>
	/// Returns ID of a block at specific position, only using World GetBlockIDAt function. No chunks need to be generated beforehand.
	/// </summary>
	Block::ID IDAt(const GeomContext& ctx, GridVec vec) const
	{
		glm::ivec3 v = GridVecToVec3(vec);
		Block::ID id = ctx.world->GetGenerator()->GetBlockIDAt(ctx.globalPos + v);
		return id;
	}

	void AddBlockFace(const GeomContext& const ctx, TextureAtlas::TextureID texid)
	{
		const Block* neighbour = At(ctx, ctx.vec);
		if (neighbour && BlockData::IsSolid(neighbour->GetID()) && !BlockData::IsTransparent(neighbour->GetID())) return;
		if (!neighbour && !BlockData::IsAir(IDAt(ctx, ctx.vec))) return; // handling chunk borders when generating for first time

		VRendererHelper<FVertex>::AddFace(*ctx.mesh->m_meshPtr, ctx.localPos, ctx.vec, texid, Block::GetLightLevelSafe(neighbour));
	}
};
