#include "amath.h"
#include <cmath>

void AABB::SetCenterTo(glm::vec2 center)
{
	glm::vec2 prevCenter = GetCenter();
	glm::vec2 diff = center - prevCenter;
	m_min += diff;
	m_max += diff;
}

bool AABB::Contains(glm::vec2 point) const
{
	return point.x >= m_min.x && point.x < m_max.x && point.y >= m_min.y && point.y < m_max.y;
}

bool AABB::Intersects(const AABB& other) const
{
	for (int i = 0; i < 4; i++)
	{
		if (Contains(other[i]))
			return true;
	}
	return false;
}

glm::vec2 AABB::operator[](int index) const
{
	//ASSERT_VAR(index, index >= 0 && index < 4, "Cannot index AABB. Index must be within the range of [0; 3]");
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

AABB AABB::FromCenterAndSize(glm::vec2 center, glm::vec2 size)
{
	//ASSERT_VAR(size.x, size.x >= 0, "Size cannot be negative");
	//ASSERT_VAR(size.y, size.y >= 0, "Size cannot be negative");
	return AABB
	(
		glm::vec2(center.x - size.x / 2, center.y - size.y / 2),
		glm::vec2(center.x + size.x / 2, center.y + size.y / 2)
	);
}

AABB AABB::FromCenterAndSize(glm::vec2 center, float size)
{
	//ASSERT_VAR(size, size >= 0, "Size cannot be negative");
	return AABB
	(
		glm::vec2(center.x - size / 2, center.y - size / 2),
		glm::vec2(center.x + size / 2, center.y + size / 2)
	);
}

bool AABB::Contains(glm::vec2 min, glm::vec2 max, glm::vec2 pos)
{
	return AABB(min, max).Contains(pos);
}

bool AABB::Intersects(const AABB& a, const AABB& b)
{
	return a.Intersects(b);
}

bool AABB::Intersects(glm::vec2 min1, glm::vec2 max1, glm::vec2 min2, glm::vec2 max2)
{
	return AABB(min1, max1).Intersects(AABB(min2, max2));
}

AABB AABB::GetIntersection(const AABB& a, const AABB& b)
{
	const float res_min_x{ fmax(a.m_min.x, b.m_min.x) };
	const float res_min_y{ fmax(a.m_min.y, b.m_min.y) };

	const float res_max_x{ fmin(a.m_max.x, b.m_max.x) };
	const float res_max_y{ fmin(a.m_max.y, b.m_max.y) };

	if (res_min_x < res_max_x && res_min_y < res_max_y)
		return AABB(glm::vec2(res_min_x, res_min_y), glm::vec2(res_max_x, res_max_y));

	return AABB();
}