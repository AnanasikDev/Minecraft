#pragma once

#include "Face.h"
#include "Vertex.h"
#include "commons.h"
#include "Mesh.h"

class RendererHelper
{
public:
	void Init();
};

template <typename Vertex>
class VRendererHelper : public RendererHelper
{
};

template <>
class VRendererHelper<FVertex>;

template <>
class VRendererHelper<FVertex> : public RendererHelper
{
public:
	template <typename Vertex>
	static void AddFace(MeshBase<Vertex>& mesh, glm::ivec3 pos, GridVec vec, TextureAtlas::TextureID texid, unsigned int light)
	{
		Face<Vertex> face = GenerateFace<Vertex>(pos, vec, texid, light, mesh.GetVerticesCount());
		mesh.AddFace(face);
	}

	template <typename Vertex>
	static Face<Vertex> GenerateFace(Vertex v00, Vertex v10, Vertex v11, Vertex v01, int index)
	{
		std::array<Vertex, 4> v{ v00, v10, v11, v01 };
		std::array<unsigned int, 6> e{ index + 3, index + 2, index, index + 2, index + 1, index };
		return Face<Vertex>{ v, e };
	}

	template <typename Vertex>
	static Face<Vertex> GenerateFace(glm::ivec3 pos, GridVec vec, TextureAtlas::TextureID texid, unsigned int light, int index)
	{
		switch (vec)
		{
		case GridVec::Back:
		{
			FVertex v00(pos.x,		pos.y,		pos.z,		1, 1, texid, light);
			FVertex v10(pos.x + 1,	pos.y,		pos.z,		0, 1, texid, light);
			FVertex v11(pos.x + 1,	pos.y + 1,	pos.z,		0, 0, texid, light);
			FVertex v01(pos.x,		pos.y + 1,	pos.z,		1, 0, texid, light);
			return GenerateFace<Vertex>(v00, v10, v11, v01, index);
		}
		case GridVec::Front:
		{
			FVertex v00(pos.x,		pos.y,		pos.z + 1,	1, 1, texid, light);
			FVertex v10(pos.x + 1,	pos.y,		pos.z + 1,	0, 1, texid, light);
			FVertex v11(pos.x + 1,	pos.y + 1,	pos.z + 1,	0, 0, texid, light);
			FVertex v01(pos.x,		pos.y + 1,	pos.z + 1,	1, 0, texid, light);
			return GenerateFace<Vertex>(v01, v11, v10, v00, index);
		}
		case GridVec::Right:
		{
			FVertex v00(pos.x + 1,	pos.y + 1,	pos.z,		1, 0, texid, light);
			FVertex v10(pos.x + 1,	pos.y,		pos.z,		1, 1, texid, light);
			FVertex v11(pos.x + 1,	pos.y,		pos.z + 1,	0, 1, texid, light);
			FVertex v01(pos.x + 1,	pos.y + 1,	pos.z + 1,	0, 0, texid, light);
			return GenerateFace<Vertex>(v00, v10, v11, v01, index);
		}
		case GridVec::Left:
		{
			FVertex v00(pos.x,		pos.y + 1,	pos.z,		1, 0, texid, light);
			FVertex v10(pos.x,		pos.y,		pos.z,		1, 1, texid, light);
			FVertex v11(pos.x,		pos.y,		pos.z + 1,	0, 1, texid, light);
			FVertex v01(pos.x,		pos.y + 1,	pos.z + 1,	0, 0, texid, light);
			return GenerateFace<Vertex>(v01, v11, v10, v00, index);
		}
		case GridVec::Top:
		{
			FVertex v00(pos.x,		pos.y + 1,	pos.z,		0, 0, texid, light);
			FVertex v10(pos.x + 1,	pos.y + 1,	pos.z,		1, 0, texid, light);
			FVertex v11(pos.x + 1,	pos.y + 1,	pos.z + 1,	1, 1, texid, light);
			FVertex v01(pos.x,		pos.y + 1,	pos.z + 1,	0, 1, texid, light);
			return GenerateFace<Vertex>(v00, v10, v11, v01, index);
		}
		case GridVec::Bottom:
		{
			FVertex v00(pos.x,		pos.y,		pos.z,		0, 0, texid, light);
			FVertex v10(pos.x + 1,	pos.y,		pos.z,		1, 0, texid, light);
			FVertex v11(pos.x + 1,	pos.y,		pos.z + 1,	1, 1, texid, light);
			FVertex v01(pos.x,		pos.y,		pos.z + 1,	0, 1, texid, light);
			return GenerateFace<Vertex>(v01, v11, v10, v00, index);
		}
		}
	}
};