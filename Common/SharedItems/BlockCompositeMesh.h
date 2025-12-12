#pragma once

#include "CompositeMesh.h"
#include "RendererHelper.h"

struct GeomContext;

template <typename Vertex>
struct BlockCompositeMesh : public BaseCompositeMesh<Vertex>
{
	void AddBlockFace(const GeomContext& const ctx, TextureAtlas::TextureID texid)
	{
		VRendererHelper<FVertex>::AddFace(*ctx.mesh->m_meshPtr, ctx.localPos, ctx.vec, texid, 15);
	}
};