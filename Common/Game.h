#pragma once
#include <memory>
#include <glm/vec2.hpp>
#include <vector>
#include <deque>

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
class BaseDebug;
class AssetManager;

class Game
{
public:
	enum class Platform
	{
		Windows,
		Linux
	};

	static constexpr unsigned int WINDOW_WIDTH = 1024 * 1.5f;
	static constexpr unsigned int WINDOW_HEIGHT = 768 * 1.5f;
	static constexpr float ASPECT_RATIO = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);

	Game(Input* const input, IGraphics* graphics, Platform platform);
	virtual ~Game();
	void Start(); 
	Input& GetInput();
	void Quit();
	inline float GetDeltaTime() const { return gameDeltaTime; }

	std::unique_ptr<Player> m_player;
	Program* program;
	Shader* fragShader;
	Shader* vertShader;
	TextureAtlas atlas;
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<World> m_world;
	std::unique_ptr<AssetManager> m_assetManager;

	inline const bool IsWindows() const
	{
		return m_platform == Platform::Windows;
	}

	inline const bool IsLinux() const
	{
		return m_platform == Platform::Linux;
	}

protected:
	void ProcessInput();
	virtual void LateUpdate(float /*gameDeltaTime*/) {}
	virtual void Render() {}
	virtual void PostRender() {}
	void DrawCrosshair();
	void DisplaySettings();
	
	Input* const input;
	bool quitting{false};
	float gameDeltaTime;

	IGraphics* graphics;

private:
	void InitializeOpenGLES();

	int frameCount{0};
	bool m_displaySettings{ true };
	float m_averageFPS{ 0 };
	int COLLECT_FPS_DATA_FRAMES = 200;
	std::deque<float> m_fpsDeque;
	std::unique_ptr<BaseDebug> m_debug;
	Platform m_platform{ Platform::Windows };
};

