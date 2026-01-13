#pragma once

#include "ConsoleBackend.h"

class Game;

class Console
{
public:
	bool m_isOpen{ false };

	void Init(Game* game);
	void Render();
	void Update();
	void Toggle(bool state);

private:
	ConsoleBackend m_backend;
	Game* m_game{ nullptr };
};