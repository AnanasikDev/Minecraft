#pragma once
#include <memory>
#include <glm/vec2.hpp>
#include <vector>

#include "glh.h"
#include "TextureAtlas.h"

class IGraphics;
class Input;
class Player;
struct Program;
struct Shader;
class BlocksDatabase;
class World;
class Renderer;

class Game
{
public:
	static constexpr unsigned int WINDOW_WIDTH = 1024;
	static constexpr unsigned int WINDOW_HEIGHT = 768;
	static constexpr float ASPECT_RATIO = 1024.0f / 768.0f;

	Game(const Input* const input, IGraphics* graphics);
	virtual ~Game();
	void Start(); 
	const Input& GetInput() const;
	void Quit();

	std::unique_ptr<Player> m_player;
	Program* program;
	Shader* fragShader;
	Shader* vertShader;
	TextureAtlas atlas;
	BlocksDatabase* blocks;
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<World> m_world;

protected:
	void ProcessInput();
	virtual void LateUpdate(float /*gameDeltaTime*/) {}
	virtual void Render() {}
	virtual void PostRender() {}
	
	const Input* const input;
	bool quitting{false};
	float gameDeltaTime;

	IGraphics* graphics;

private:
	void InitializeOpenGLES();
	void ClearScreen();

	int frameCount{0};
};

