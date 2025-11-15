#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "TextureAtlas.h"

template <typename Txyz, typename Tuv>
struct AbstractVertex
{
	Txyz x;
	Txyz y;
	Txyz z;
	Tuv u;
	Tuv v;
	TextureAtlas::TextureID texid;

	AbstractVertex(Txyz x, Txyz y, Txyz z, Tuv u, Tuv v, TextureAtlas::TextureID texid) : x(x), y(y), z(z), u(u), v(v), texid(texid)
	{
	}

	void Add(Txyz dx, Txyz dy, Txyz dz)
	{
		x += dx;
		y += dy;
		z += dz;
	}

	static constexpr unsigned int stride = 
		sizeof(Txyz) * 3 + 
		sizeof(Tuv) * 2 + 
		sizeof(TextureAtlas::TextureID);
};

using FVertex = AbstractVertex<float, float>;