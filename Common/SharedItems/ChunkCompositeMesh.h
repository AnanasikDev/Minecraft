#pragma once

#include "commons.h"
#include "CompositeMesh.h"
#include "Block.h"
#include "RendererHelper.h"
#include "BlockData.h"

class Chunk;
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
		return ctx.request->GetBlockAtWorld(ctx.globalPos + v);
	}

	/// <summary>
	/// Returns block instance at specific position, nullptr if outside the chunk.
	/// </summary>
	const Block* GetBlock(const GeomContext& ctx) const
	{
		const Block* result = ctx.request->chunk->AtSafe(ctx.localPos);
		return result;
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

	void AddBlockFace(const GeomContext& ctx, TextureAtlas::TextureID texid)
	{
		const Block* const neighbour { At(ctx, ctx.vec) };
		const Block* const self		 { GetBlock(ctx) };
		const Block::ID selfID		 { self->GetID() };
		
		if (!neighbour) return; // handling chunk borders when generating for first time
		
		const Block::ID neighID{ neighbour->GetID() };

		const bool skip{
			(BlockData::IsSolid(neighID) && !BlockData::IsTransparent(neighID))
			|| 
			(BlockData::IsWater(selfID) && BlockData::IsWater(neighID))
		};
		if (skip) return;

		unsigned char blockLightLevel{ 0 };
		if (self->GetEmission() == 0 && neighbour)
		{ 
			blockLightLevel = neighbour->GetTotalLightPacked();
		}
		else
		{
			blockLightLevel = self->GetTotalLightPacked();
		}

		VRendererHelper<FVertex>::AddFace(*ctx.mesh->m_meshPtr, ctx.localPos, ctx.vec, texid, blockLightLevel);
	}
};
