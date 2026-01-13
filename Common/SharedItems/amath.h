#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
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

	static inline bool RreNearlyEqual(float a, float b, float epsilon = 10e-6f)
	{
		return std::abs(a - b) <= epsilon;
	}

	static inline glm::vec3 CastFloat(glm::ivec3 ivec)
	{
		return glm::vec3(static_cast<float>(ivec.x), static_cast<float>(ivec.y), static_cast<float>(ivec.z));
	}
};

struct AABB2D
{
	glm::vec2 m_min{ 0, 0 };
	glm::vec2 m_max{ 0, 0 };

	AABB2D() = default;
	explicit AABB2D(glm::vec2 min, glm::vec2 max) : m_min(min), m_max(max) {}

	void SetCenterTo(glm::vec2 center);
	glm::vec2 GetCenter() const { return (m_max + m_min) / 2.0f; }
	glm::vec2 GetSize() const { return m_max - m_min; }
	float GetArea() const { glm::vec2 s = GetSize(); return s.x * s.y; }
	bool IsEmpty() const { return m_max.x - m_min.x == 0 || m_max.y - m_min.y == 0; }

	static AABB2D FromCenterAndSize(glm::vec2 center, glm::vec2 size);
	static AABB2D FromCenterAndSize(glm::vec2 center, float size);

	static bool Contains(glm::vec2 min, glm::vec2 max, glm::vec2 pos);
	static bool Intersects(glm::vec2 min1, glm::vec2 max1, glm::vec2 min2, glm::vec2 max2);
	static bool Intersects(const AABB2D& a, const AABB2D& b);

	static AABB2D GetIntersection(const AABB2D& a, const AABB2D& b);
	bool Contains(glm::vec2 pofloat) const;
	bool Intersects(const AABB2D& other) const;

	glm::vec2 operator[](int index) const;
};

struct AABB3D
{
	glm::vec3 m_min{ 0 };
	glm::vec3 m_max{ 0 };

	AABB3D() = default;
	explicit AABB3D(glm::vec3 min, glm::vec3 max) : m_min(min), m_max(max) {}

	void SetCenterTo(glm::vec3 center);
	glm::vec3 GetCenter() const { return (m_max + m_min) / 2.0f; }
	glm::vec3 GetSize() const { return m_max - m_min; }
	float GetVolume() const { glm::vec3 s = GetSize(); return s.x * s.y * s.z; }
	bool IsEmpty() const
	{
		return AMath::RreNearlyEqual(m_max.x, m_min.x) || AMath::RreNearlyEqual(m_max.y, m_min.y)
			|| AMath::RreNearlyEqual(m_max.z, m_min.z);
	}

	static AABB3D FromCenterAndSize(glm::vec3 center, glm::vec3 size);
	static AABB3D FromCenterAndSize(glm::vec3 center, float size);

	static bool Contains(glm::vec3 min, glm::vec3 max, glm::vec3 pos);
	static bool Intersects(glm::vec3 min1, glm::vec3 max1, glm::vec3 min2, glm::vec3 max2);
	static bool Intersects(const AABB3D& a, const AABB3D& b);

	static AABB3D GetIntersection(const AABB3D& a, const AABB3D& b);
	bool Contains(glm::vec3 pofloat) const;
	bool Intersects(const AABB3D& other) const;

	glm::vec3 operator[](int index) const;
};