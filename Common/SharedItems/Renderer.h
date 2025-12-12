#pragma once

#include "RenderRequest.h"
#include <unordered_map>
#include <memory>

class Game;
struct Shader;
struct Program;

class Renderer
{
public:
	Game* m_game{ nullptr };
	RENDER_MODE m_mode{RENDER_MODE::SOLID_MODE};

	Renderer() = default;
	~Renderer() = default;

	template <typename Vertex>
	void Push(RenderRequest<Vertex> request);
};

#include "Renderer.tpp"