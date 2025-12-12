#pragma once

#include <array>

template <typename Vertex>
struct Face
{
	std::array<Vertex, 4> m_vertices;
	std::array<unsigned int, 6> m_indices;
};
