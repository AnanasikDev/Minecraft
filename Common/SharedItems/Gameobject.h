#pragma once

#include "Transform.h"

class Game;

class Gameobject
{
public:
	Game* m_game{ nullptr };
	Transform m_transform;

	Gameobject() = default;
	Gameobject(Game* game) : m_game(game)
	{
	}
	Gameobject(Game* game, Transform transform) : m_game(game), m_transform(transform)
	{
	}
	virtual void Update();
};