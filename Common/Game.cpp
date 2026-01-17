#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ImGui-master/imgui.h"

#include "Random.h"

#include "Sunmoon.h"
#include "World.h"
#include "Game.h"
#include "Input.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "IGraphics.h"
#include "IInput.h"
#include "Player.h"
#include "Camera.h"
#include "Program.h"
#include "Shader.h"
#include "Chunk.h"
#include "Renderer.h"
#include "Block.h"
#include "VertexBuffer.h"
#include "RendererHelper.h"
#include "BaseDebug.h"
#include "OpenGLDebug.h"
#include "AssetManager.h"

bool BaseDebug::show = false;

Game::Game(Input* const input, IGraphics* graphics, Platform platform) :
	m_input(input),
	m_graphics(graphics),
	m_platform(platform)
{
	m_renderer = std::make_unique<Renderer>();
	m_renderer->m_game = this;
	m_debug = std::make_unique<OpenGLDebug>();
}

Game::~Game()
{

}

void Game::Start()
{
	InitializeOpenGLES();
	printf("This cross project was partly inspired by BUas Student Ferri de Lange\n");
	printf("This GPU supplied by  :%s\n", glGetString(GL_VENDOR));
	printf("This GPU supports GL  :%s\n", glGetString(GL_VERSION));
	printf("This GPU Renders with :%s\n", glGetString(GL_RENDERER));
	printf("This GPU Shaders are  :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	Saver::LoadGamerules(m_gamerules);

	GameTime::Init();
	GameTime::SetDayTime(m_gamerules.m_defaultTime);

	RendererHelper rendererHelper;
	rendererHelper.Init();
	m_renderer->m_helper = &rendererHelper;
	m_renderer->m_helper->SetupPrograms(m_assetManager.get());

	Random::Init();

	// Timing
	auto startTime = std::chrono::system_clock::now();
	auto lastTime = startTime;

	m_atlas = std::make_unique<TextureAtlas>();
	m_atlas->Create3D(m_assetManager->GetAssetPathString("Textures/atlas.png"), GL_REPEAT, 16);

	IMouse& mouse = GetInput().GetMouse();
	mouse.Init();

	BlocksDatabase::Init(this);

	m_world = std::make_unique<World>();
	m_world->SetGenerator(std::make_unique<WorldGen>(m_world.get()));
	m_world->Init(this);

	m_UI.Init(this);

	m_player = std::make_unique<Player>(this);

	m_sunmoon = std::make_unique<Sunmoon>();
	m_sunmoon->Init(this);

	m_graphics->InitGUI();

	GameTime::ToggleDayCycle(m_gamerules.m_doDayNightCycle);

	m_saver.LoadGlobal(this);

	while(!m_isQuitting)
	{
		m_graphics->BeginFrame();
		ProcessInput();
		if (m_input->GetKeyboard().IsKeyPressed(Key::ESCAPE))
		{
			ImGuiIO& io = ImGui::GetIO();
			if (!io.WantCaptureKeyboard)
			{
				m_graphics->Quit();
				OnQuit();
				return;
			}
		}

		auto time = std::chrono::system_clock::now();
		std::chrono::duration<float> delta = time - lastTime;

		m_gameDeltaTime = delta.count();

		std::chrono::duration<float> elapsed = time - startTime;
		if(elapsed.count() > 0.25f && m_frameCount > 10)
		{
			m_averageFPS = static_cast<float>(m_frameCount) / elapsed.count();
			startTime = time;
			m_frameCount = 0;
			//printf("%f\n", m_averageFPS);
		}
		
		if (m_input->GetKeyboard().IsKeyPressed(Key::TAB))
		{
			if (m_renderer->m_mode == RENDER_MODE::SOLID_MODE)
			{
				m_renderer->m_mode = RENDER_MODE::WIREFRAME_MODE;
			}
			else if (m_renderer->m_mode == RENDER_MODE::WIREFRAME_MODE)
			{
				m_renderer->m_mode = RENDER_MODE::SOLID_MODE;
			}
		}

		if (m_input->GetKeyboard().IsKeyPressed(Key::C))
		{
			ImGuiIO& io = ImGui::GetIO();
			if (!io.WantCaptureKeyboard)
			{
				if (m_graphics->IsCursorLocked()) m_graphics->UnlockCursor();
				else m_graphics->LockCursor();
				GetInput().GetMouse().Init();
			}
		}

		if (m_input->GetKeyboard().IsKeyPressed(Key::T))
		{
			ImGuiIO& io = ImGui::GetIO();
			if (!io.WantCaptureKeyboard)
			{
				m_displaySettings = !m_displaySettings;
			}
		}

		m_renderer->Get()->ClearScreen();
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		m_player->Update();
		m_world->Update();
		m_world->FixedUpdate();
		m_sunmoon->Update(m_player.get());

		m_renderer->m_helper->Update(m_gamerules);
		m_renderer->m_helper->Update(m_gamerules);
		m_sunmoon->Render(m_player.get());
		m_world->Render(m_player.get());
		m_player->Render();
		DrawCrosshair();
		if (m_displaySettings) DisplaySettings();
		m_UI.Update();
		m_UI.Render();
		m_graphics->EndFrame();

		GetInput().GetKeyboard().Update();
		GetInput().GetMouse().Flush();
		m_graphics->SwapBuffer();

		lastTime = time;
		++m_frameCount;
		m_fpsDeque.push_back(1.0f / m_gameDeltaTime);
		if (static_cast<int>(m_fpsDeque.size()) == m_fpsCollectionNumber)
		{
			m_fpsDeque.pop_front();
		}

		VertexBuffer<FVertex>::NUM_RENDERED = 0;
	}

	m_graphics->Quit();
	OnQuit();
}

Input& Game::GetInput()
{
	return *m_input;
}

IGraphics* Game::GetGraphics()
{
	return m_graphics;
}

void Game::Quit()
{
	m_isQuitting = true;
}

void Game::ProcessInput()
{
	IMouse& mouse = GetInput().GetMouse();

	mouse.Update();
}

void Game::DrawCrosshair()
{
	ImU32 color{ IM_COL32(255, 255, 255, 255) };
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	draw_list->AddLine(ImVec2(center.x - 10, center.y), ImVec2(center.x + 10, center.y), color, 2.0f);
	draw_list->AddLine(ImVec2(center.x, center.y - 10), ImVec2(center.x, center.y + 10), color, 2.0f);
}

void Game::DrawUI()
{
	m_UI.Render();
}

void Game::DisplaySettings()
{
	ImGui::PushItemWidth(160);
	glm::vec3 playerPos = m_player->m_transform.GetWorldPosition();
	ImGui::SeparatorText("Graphics");
	ImGui::DragFloat("FPS", &m_averageFPS, 0.0f, 0.0f, 0.0f, "%.1f", ImGuiSliderFlags_NoInput);
	std::vector<float> values;
	for (std::deque<float>::iterator it = m_fpsDeque.begin(); it != m_fpsDeque.end(); ++it)
	{
		values.push_back(*it);
	}
	ImVec2 size(10, 20);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, size);
	ImGui::PlotLines("Graph", values.data(), static_cast<int>(values.size()), 0, "avg", 0.0f, 120.0f);
	ImGui::SameLine();
	ImGui::PushItemWidth(30);
	if (ImGui::DragInt("Num", &m_fpsCollectionNumber))
	{
		m_fpsDeque.clear();
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	int verts = VertexBuffer<FVertex>::NUM_RENDERED;
	ImGui::DragInt("Vertices", &verts, 0.0f, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::SliderInt("Generation distance", &GetGamerules().m_generationDistance, 2, 32);
	ImGui::DragInt("Chunks active", &m_world->m_chunksActive, 0.0f, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::DragInt("Chunks rendered", &m_world->m_chunksRendered, 0.0f, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::Checkbox("Show debug", &BaseDebug::show);
	ImGui::DragFloat("Frustum culling", &Camera::FOV_MULT, 0.03f, 0.3f, 1.5f, "%.2f");
	ImGui::Checkbox("Render water", &GetGamerules().m_renderWater);

	ImGui::SeparatorText("Player");
	glm::ivec3 playerLocalPos = World::WorldToLocalAny(World::SnapToBlock(m_player->m_transform.GetWorldPosition()));
	ImGui::DragFloat3("Player world position", &playerPos.x, 0.0f, 0.0f, 0.0f, "%.1f", ImGuiSliderFlags_NoInput);
	ImGui::DragInt3("Player local position", &playerLocalPos.x, 0.0f, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	glm::ivec3 playerChunkPos = World::WorldBlockToChunkGrid(m_player->m_transform.GetWorldPosition());
	glm::ivec3 playerBlockPos = World::SnapToBlock(m_player->m_transform.GetWorldPosition());
	ImGui::DragInt3("Player chunk position", &playerChunkPos.x, 0.0f, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::SliderFloat("Player speed", &m_player->m_baseMovementSpeed, 4.0f, 18.0f);

	BlockGenData data = m_world->GetGenerator()->GetBlockGenDataAt(playerBlockPos);
	float temp = data.temperature;
	float humidity = data.humidity;
	ImGui::SeparatorText("Worldgen");
	ImGui::DragFloat("Temperature", &temp, 0.0f, 0.0f, 0.0f, "%.3f", ImGuiSliderFlags_NoInput);
	ImGui::DragFloat("Humidity", &humidity, 0.0f, 0.0f, 0.0f, "%.3f", ImGuiSliderFlags_NoInput);
	std::string biom = "Biom: " + WorldGen::GetBiomName(data.biom);
	ImGui::Text(biom.c_str());
	std::string iscave = data.IsCave() ? "Cave" : "Non-cave";
	ImGui::Text(iscave.c_str());
	ImGui::DragFloat("Caveness", &data.caveness, 0.0f, 0.0f, 0.0f, "%.3f", ImGuiSliderFlags_NoInput);
	std::string iswater = data.isWater ? "Water" : "Not water";
	ImGui::Text(iswater.c_str());

	glm::vec3 rightHandPos = m_player->m_rightHand.m_transform.GetLocalPosition();
	ImGui::DragFloat3("Right hand", &rightHandPos.x, 0.05f);
	m_player->m_rightHand.m_transform.SetLocalPosition(rightHandPos);

	Block* block = m_world->GetBlockAtWorld(playerBlockPos);
	int light = -1;
	int sky = -1;
	std::string name = "-";
	if (block)
	{
		light = static_cast<int>(block->GetLightLevel());
		sky = static_cast<int>(block->GetSkyExposure());
		name = BlocksDatabase::Get(block->GetID())->m_name;
	}
	ImGui::DragInt("Light", &light, 0, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::DragInt("Sky exposure", &sky, 0, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::Text(name.c_str());

	if (ImGui::Checkbox("Do day/night", &m_gamerules.m_doDayNightCycle))
	{
		GameTime::ToggleDayCycle(m_gamerules.m_doDayNightCycle);
	}

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << GameTime::TimeOfDay();
	std::string stime = ss.str();
	ImGui::Text(stime.c_str());
	ImGui::DragFloat("Day duration", &GameTime::DAY_SECONDS, 1.0f, 3.0f, 100.0f, "%.3f");
}

void Game::InitializeOpenGLES()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0.0f, 1.0f);
	glClearDepthf(1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Game::OnQuit()
{
	Saver::SaveGamerules(m_gamerules);
	m_saver.SaveGlobal(this);
	m_world->OnQuit();
}
