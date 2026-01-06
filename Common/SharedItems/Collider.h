#pragma once

#include "amath.h"
#include <vector>

struct Intersection
{
	bool m_exists{ false };
	glm::vec3 m_size;
};

struct Collider
{
	std::vector<AABB3D> m_boxes;

	Collider() = default;
	Collider(std::vector<AABB3D> boxes);

	Intersection GetIntersection(const Collider& other) const;

	static Collider FullBlock();

	void AddBox(AABB3D aabb);
	void AddBox(glm::vec3 min, glm::vec3 max);
};