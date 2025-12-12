#pragma once

#include "Transform.h"

class Game;

class Gameobject
{
public:
	Transform m_transform;
	Game* m_game{ nullptr };

	Gameobject() = default;
	Gameobject(Game* game) : m_game(game)
	{
	}
	Gameobject(Game* game, Transform transform) : m_game(game), m_transform(transform)
	{
	}
	virtual void Update();
};