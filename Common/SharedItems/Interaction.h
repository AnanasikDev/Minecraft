#pragma once

#include "ChunkProvider.h"
#include "Ray.h"

class Player;
struct ChunkProvider;

/// <summary>
/// POD for block/entity interaction result
/// </summary>
struct Interaction
{
	Player* player{ nullptr };
	ChunkProvider world;
	RaycastResult raycast;

	enum class Result : unsigned char
	{
		Success_Stop = 0,
		Failed_Continue = 1
	};
};