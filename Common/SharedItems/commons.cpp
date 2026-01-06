#include "commons.h"

glm::ivec3 GridVecToVec3(GridVec vec)
{
	switch (vec)
	{
	case GridVec::Right:	return glm::ivec3(1, 0, 0);
	case GridVec::Left:		return glm::ivec3(-1, 0, 0);
	case GridVec::Top:		return glm::ivec3(0, 1, 0);
	case GridVec::Bottom:	return glm::ivec3(0, -1, 0);
	case GridVec::Front:	return glm::ivec3(0, 0, 1);
	case GridVec::Back:		return glm::ivec3(0, 0, -1);

	default:				return glm::ivec3(0, 0, 0);
	}
}

GridVec Vec3ToGridVec(glm::ivec3 vec)
{
	if (vec == glm::ivec3(0, 0, 1)) return GridVec::Front;
	if (vec == glm::ivec3(0, 0, -1)) return GridVec::Back;
	if (vec == glm::ivec3(0, 1, 0)) return GridVec::Top;
	if (vec == glm::ivec3(0, -1, 0)) return GridVec::Bottom;
	if (vec == glm::ivec3(1, 0, 0)) return GridVec::Right;
	if (vec == glm::ivec3(-1, 0, 0)) return GridVec::Left;

	return GridVec::Invalid;
}

GridVec InvertGridVec(GridVec vec)
{
	switch (vec)
	{
	case GridVec::Right:	return GridVec::Left;
	case GridVec::Left:		return GridVec::Right;
	case GridVec::Front:	return GridVec::Back;
	case GridVec::Back:		return GridVec::Front;
	case GridVec::Top:		return GridVec::Bottom;
	case GridVec::Bottom:	return GridVec::Top;

	default: return GridVec::Invalid;
	}
}

glm::ivec3 InvertVec3(glm::ivec3 vec)
{
	return -vec;
}

float GameTime::DAY_SECONDS = 10;
Timer GameTime::m_Timer;
