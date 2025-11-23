#pragma once

#include "RenderRequest.h"

class Game;

class Renderer
{
public:
	Game* m_game;
	RENDER_MODE m_mode{RENDER_MODE::SOLID_MODE};

	template <typename Vertex>
	void Push(RenderRequest<Vertex> request);
};

#include "Renderer.tpp"