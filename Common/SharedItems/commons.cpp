#include "commons.h"

glm::ivec3 GridVecToVec3(GridVec vec)
{
	switch (vec)
	{
	case GridVec::Invalid:
		return glm::ivec3(0, 0, 0);
	case GridVec::Right:
		return glm::ivec3(1, 0, 0);
	case GridVec::Left:
		return glm::ivec3(-1, 0, 0);
	case GridVec::Top:
		return glm::ivec3(0, 1, 0);
	case GridVec::Bottom:
		return glm::ivec3(0, -1, 0);
	case GridVec::Front:
		return glm::ivec3(0, 0, 1);
	case GridVec::Back:
		return glm::ivec3(0, 0, -1);
	}
}