#pragma once

#include "TextureAtlas.h"
#include "GeomContext.h"

template <typename Vertex>
struct Mesh;
struct GeomContext;

template <typename Vertex>
struct BaseCompositeMesh
{
	Mesh<Vertex>* m_meshPtr;

	virtual void AddBlockFace(const GeomContext& const ctx, TextureAtlas::TextureID texid) = 0;
};
