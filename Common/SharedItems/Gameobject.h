#pragma once

#include "Transform.h"

class Game;

class Gameobject
{
public:
	Transform m_transform;
	Game* m_game;

	Gameobject() = default;
	Gameobject(Game* game) : m_game(game)
	{
	
	}
	virtual void Update();
};