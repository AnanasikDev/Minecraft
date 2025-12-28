#pragma once

#include "RenderRequest.h"
#include <unordered_map>
#include <memory>

class Game;
struct Shader;
struct Program;
class RendererHelper;

class Renderer
{
public:
	Game* m_game{ nullptr };
	RENDER_MODE m_mode{RENDER_MODE::SOLID_MODE};
	RendererHelper* m_helper;

	Renderer() = default;
	~Renderer() = default;

	template <typename Vertex>
	void Push(RenderRequest<Vertex> request);

	inline RendererHelper* Get()
	{
		return m_helper;
	}
};

#include "Renderer.tpp"