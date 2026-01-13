#pragma once

#include <memory>

class Hotbar;
class Healthbar;
class Game;
class TextureAtlas;
class Console;

class UI
{
public:
	UI();
	~UI();

	void Init(Game* game);
	void Render();
	void Update();

	std::unique_ptr<Hotbar> m_hotbar;
	std::unique_ptr<Healthbar> m_healthbar;
	std::unique_ptr<Console> m_console;

private:
	Game* m_game{ nullptr };
	std::unique_ptr<TextureAtlas> m_itemsAtlas;
};