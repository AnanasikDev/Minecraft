#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <vector>
#include <deque>
#include "glh.h"
#include "TextureAtlas.h"
#include "Gamerules.h"
#include "Saver.h"
#include "UI.h"

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
class Sunmoon;

class Game
{
public:
	enum class Platform
	{
		Windows,
		Linux
	};

	static constexpr unsigned int WINDOW_WIDTH	{ 2560 };
	static constexpr unsigned int WINDOW_HEIGHT	{ 1440 };
	static constexpr float ASPECT_RATIO{ static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT) };
	static constexpr glm::vec3 SKY_DAY_COLOR	{ 0.733f, 0.902f, 0.969f };
	static constexpr glm::vec3 SKY_NIGHT_COLOR	{ 0.180f, 0.243f, 0.271f };

	Game(Input* const input, IGraphics* graphics, Platform platform);
	virtual ~Game();
	void Start();
	Input& GetInput();
	IGraphics* GetGraphics();
	void Quit();
	void OnQuit();
	inline float GetDeltaTime() const { return m_gameDeltaTime; }

	std::unique_ptr<Player> m_player;
	std::unique_ptr<TextureAtlas> m_atlas;
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<World> m_world;
	std::unique_ptr<AssetManager> m_assetManager;
	std::unique_ptr<Sunmoon> m_sunmoon;

	Saver m_saver;

	inline const bool IsWindows() const
	{
		return m_platform == Platform::Windows;
	}

	inline const bool IsLinux() const
	{
		return m_platform == Platform::Linux;
	}

	inline Gamerules& GetGamerules()
	{
		return m_gamerules;
	}

	inline UI& GetUI()
	{
		return m_UI;
	}

protected:
	void ProcessInput();
	virtual void LateUpdate(float /*gameDeltaTime*/) {}
	virtual void Render() {}
	virtual void PostRender() {}
	void DrawCrosshair();
	void DrawUI();
	void DisplaySettings();
	
	Input* const m_input{ nullptr };
	bool m_isQuitting{ false };
	float m_gameDeltaTime{ 0.0f };

	IGraphics* m_graphics{ nullptr };

private:
	void InitializeOpenGLES();

	int m_frameCount{ 0 };
	bool m_displaySettings{ true };
	float m_averageFPS{ 0 };
	int m_fpsCollectionNumber{ 200 };
	std::deque<float> m_fpsDeque;
	std::unique_ptr<BaseDebug> m_debug;
	Platform m_platform{ Platform::Windows };

	Gamerules m_gamerules;
	UI m_UI;
};

