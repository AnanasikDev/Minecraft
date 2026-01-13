#include <cmath>
#include <algorithm>
#include "amath.h"

void AABB2D::SetCenterTo(glm::vec2 center)
{
	glm::vec2 prevCenter = GetCenter();
	glm::vec2 diff = center - prevCenter;
	m_min += diff;
	m_max += diff;
}

bool AABB2D::Contains(glm::vec2 point) const
{
	return point.x >= m_min.x && point.x < m_max.x && point.y >= m_min.y && point.y < m_max.y;
}

bool AABB2D::Intersects(const AABB2D& other) const
{
	for (int i = 0; i < 4; i++)
	{
		if (Contains(other[i]))
			return true;
	}
	return false;
}

glm::vec2 AABB2D::operator[](int index) const
{
	assert(index >= 0 && index < 4 && "Cannot index AABB. Index must be within the range of [0; 3]");
	switch (index)
	{
	case 0:
		return m_min;
		break;
	case 1:
		return glm::vec2(m_min.x, m_max.y);
		break;
	case 2:
		return m_max;
		break;
	case 3:
		return glm::vec2(m_max.x, m_min.y);
		break;
	default:
		return glm::vec2();
	}
}

AABB2D AABB2D::FromCenterAndSize(glm::vec2 center, glm::vec2 size)
{
	assert(size.x >= 0 && "Size cannot be negative");
	assert(size.y >= 0 && "Size cannot be negative");
	return AABB2D
	(
		glm::vec2(center.x - size.x / 2, center.y - size.y / 2),
		glm::vec2(center.x + size.x / 2, center.y + size.y / 2)
	);
}

AABB2D AABB2D::FromCenterAndSize(glm::vec2 center, float size)
{
	assert(size >= 0 && "Size cannot be negative");
	return AABB2D
	(
		glm::vec2(center.x - size / 2, center.y - size / 2),
		glm::vec2(center.x + size / 2, center.y + size / 2)
	);
}

bool AABB2D::Contains(glm::vec2 min, glm::vec2 max, glm::vec2 pos)
{
	return AABB2D(min, max).Contains(pos);
}

bool AABB2D::Intersects(const AABB2D& a, const AABB2D& b)
{
	return a.Intersects(b);
}

bool AABB2D::Intersects(glm::vec2 min1, glm::vec2 max1, glm::vec2 min2, glm::vec2 max2)
{
	return AABB2D(min1, max1).Intersects(AABB2D(min2, max2));
}

AABB2D AABB2D::GetIntersection(const AABB2D& a, const AABB2D& b)
{
	const float res_min_x{ fmaxf(a.m_min.x, b.m_min.x) };
	const float res_min_y{ fmaxf(a.m_min.y, b.m_min.y) };

	const float res_max_x{ fminf(a.m_max.x, b.m_max.x) };
	const float res_max_y{ fminf(a.m_max.y, b.m_max.y) };

	if (res_min_x < res_max_x && res_min_y < res_max_y)
		return AABB2D(glm::vec2(res_min_x, res_min_y), glm::vec2(res_max_x, res_max_y));

	return AABB2D();
}

void AABB3D::SetCenterTo(glm::vec3 center)
{
	glm::vec3 prevCenter = GetCenter();
	glm::vec3 diff = center - prevCenter;
	m_min += diff;
	m_max += diff;
}

bool AABB3D::Contains(glm::vec3 point) const
{
	return point.x >= m_min.x && point.x < m_max.x &&
		point.y >= m_min.y && point.y < m_max.y &&
		point.z >= m_min.z && point.z < m_max.z;
}

bool AABB3D::Intersects(const AABB3D& other) const
{
	return (m_min.x <= other.m_max.x && m_max.x >= other.m_min.x) &&
		(m_min.y <= other.m_max.y && m_max.y >= other.m_min.y) &&
		(m_min.z <= other.m_max.z && m_max.z >= other.m_min.z);
}

glm::vec3 AABB3D::operator[](int index) const
{
	switch (index)
	{
	case 0: return m_min;									// 000
	case 1: return glm::vec3(m_min.x, m_min.y, m_max.z);	// 001
	case 2: return glm::vec3(m_min.x, m_max.y, m_min.z);	// 010
	case 3: return glm::vec3(m_min.x, m_max.y, m_max.z);	// 011
	case 4: return glm::vec3(m_max.x, m_min.y, m_min.z);	// 100
	case 5: return glm::vec3(m_max.x, m_min.y, m_max.z);	// 101
	case 6: return glm::vec3(m_max.x, m_max.y, m_min.z);	// 110
	case 7: return m_max;									// 111
	default: return glm::vec3(0.0f);
	}
}

AABB3D AABB3D::FromCenterAndSize(glm::vec3 center, glm::vec3 size)
{
	glm::vec3 halfSize = size / 2.0f;
	return AABB3D(center - halfSize, center + halfSize);
}

AABB3D AABB3D::FromCenterAndSize(glm::vec3 center, float size)
{
	float halfSize = size / 2.0f;
	return AABB3D
	(
		glm::vec3(center.x - halfSize, center.y - halfSize, center.z - halfSize),
		glm::vec3(center.x + halfSize, center.y + halfSize, center.z + halfSize)
	);
}

bool AABB3D::Contains(glm::vec3 min, glm::vec3 max, glm::vec3 pos)
{
	return AABB3D(min, max).Contains(pos);
}

bool AABB3D::Intersects(const AABB3D& a, const AABB3D& b)
{
	return a.Intersects(b);
}

bool AABB3D::Intersects(glm::vec3 min1, glm::vec3 max1, glm::vec3 min2, glm::vec3 max2)
{
	return AABB3D(min1, max1).Intersects(AABB3D(min2, max2));
}

AABB3D AABB3D::GetIntersection(const AABB3D& a, const AABB3D& b)
{
	const float res_min_x{ fmaxf(a.m_min.x, b.m_min.x) };
	const float res_min_y{ fmaxf(a.m_min.y, b.m_min.y) };
	const float res_min_z{ fmaxf(a.m_min.z, b.m_min.z) };

	const float res_max_x{ fminf(a.m_max.x, b.m_max.x) };
	const float res_max_y{ fminf(a.m_max.y, b.m_max.y) };
	const float res_max_z{ fminf(a.m_max.z, b.m_max.z) };

	if (res_min_x < res_max_x && res_min_y < res_max_y && res_min_z < res_max_z)
	{
		return AABB3D(
			glm::vec3(res_min_x, res_min_y, res_min_z),
			glm::vec3(res_max_x, res_max_y, res_max_z)
		);
	}

	return AABB3D();
}