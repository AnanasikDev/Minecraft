#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "TextureAtlas.h"
#include "BufferLayout.h"

template <typename Txyz>
struct BaseVertex
{
	Txyz x, y, z;

	BaseVertex(Txyz x, Txyz y, Txyz z) : x(x), y(y), z(z)
	{
	}

	inline void Add(Txyz dx, Txyz dy, Txyz dz)
	{
		x += dx;
		y += dy;
		z += dz;
	}

	static constexpr inline unsigned int GetStride() 
	{ 
		return sizeof(Txyz) * 3;
	}

	static void SetLayout(BufferLayout* layout)
	{
		layout->PushAttribute<Txyz>(3, GetStride());
	}
};

template <typename Txyz, typename Tuv, typename Tlight>
struct ModelVertex : public BaseVertex<Txyz>
{
	Tuv u, v;
	TextureAtlas::TextureID texid;
	Tlight m_light;

	ModelVertex(Txyz x, Txyz y, Txyz z, Tuv u, Tuv v, TextureAtlas::TextureID texid, Tlight m_light) : BaseVertex<Txyz>(x, y, z), u(u), v(v), texid(texid), m_light(m_light)
	{
	}

	static constexpr inline unsigned int GetStride()
	{
		return sizeof(ModelVertex<Txyz, Tuv, Tlight>);
	}

	static void SetLayout(BufferLayout* layout)
	{
		layout->PushAttribute<Txyz>(3, GetStride());
		layout->PushAttribute<Tuv>(2, GetStride());
		layout->PushAttribute<int>(1, GetStride());
		layout->PushAttribute<int>(1, GetStride());
	}
};

using FVertex = ModelVertex<float, float, int>;
using DebugVertex = BaseVertex<float>;
