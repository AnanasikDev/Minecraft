#pragma once

#include <glm/vec2.hpp>
#include "TextureAtlas.h"

class Game;

class UIElement
{
public:
	std::string m_name;
	glm::vec2 m_position{ 0, 0 };
	glm::vec2 m_size{ 100, 100 };
	float m_scale{ 1.0f };
	int m_index{ 0 };

	UIElement(Game* game) : m_game(game)
	{
	}
	virtual ~UIElement() = default;

	virtual void Init() {};
	void LoadSprite(std::string filename);
	inline glm::vec2 GetFinalSize() const
	{
		return m_size * m_scale;
	}
	virtual void Render();
	virtual void Render(glm::vec2 uv0, glm::vec2 uv1);
	Texture& GetTextureRef();
	void AutoName();

protected:
	Texture m_texture;
	Game* m_game{ nullptr };
};