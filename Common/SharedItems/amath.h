#pragma once

#include "glm/vec2.hpp"
#include <cmath>

class AMath
{
public:
    static constexpr inline float Remap(float value, float minFrom, float maxFrom, float minTo, float maxTo)
    {
        return minTo + (maxTo - minTo) * (value - minFrom) / (maxFrom - minFrom);
    }
    static constexpr inline int PositiveMod(int n, int mod)
    {
        return (n % mod + mod) % mod;
    }

	static inline int Repeat(int t, int length)
	{
		if (t < 0)
			return (length + t) % length;
		return t % length;
	}

	static inline float Repeat(float t, float length)
	{
		return t - std::floor(t / length) * length;
	}

	static inline int PingPong(int t, int length)
	{
		length--;
		if (length == 0) return 0;
		const int mod = Repeat(t, length * 2);
		if (mod < length)
		{
			return mod;
		}
		return length - (mod - length);
	}

	static inline float PingPong(float t, float length)
	{
		length--;
		if (length == 0) return 0;
		const float mod = Repeat(t, length * 2.0f);
		return length - std::fabsf(mod - length);
	}

	static inline bool RreNearlyEqual(float a, float b, float epsilon = 10e-6)
	{
		return std::abs(a - b) <= epsilon;
	}
};

struct AABB
{
	glm::vec2 m_min{ 0, 0 };
	glm::vec2 m_max{ 0, 0 };

	AABB() = default;
	AABB(glm::vec2 min, glm::vec2 max) : m_min(min), m_max(max) {}

	void SetCenterTo(glm::vec2 center);
	glm::vec2 GetCenter() const { return (m_max + m_min) / 2.0f; }
	glm::vec2 GetSize() const { return m_max - m_min; }
	float GetArea() const { glm::vec2 s = GetSize(); return s.x * s.y; }
	bool IsEmpty() const { return m_max.x - m_min.x == 0 || m_max.y - m_min.y == 0; }

	static AABB FromCenterAndSize(glm::vec2 center, glm::vec2 size);
	static AABB FromCenterAndSize(glm::vec2 center, float size);

	static bool Contains(glm::vec2 min, glm::vec2 max, glm::vec2 pos);
	static bool Intersects(glm::vec2 min1, glm::vec2 max1, glm::vec2 min2, glm::vec2 max2);
	static bool Intersects(const AABB& a, const AABB& b);

	static AABB GetIntersection(const AABB& a, const AABB& b);
	bool Contains(glm::vec2 pofloat) const;
	bool Intersects(const AABB& other) const;

	glm::vec2 operator[](int index) const;
};