#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ImGui-master/imgui.h"

#include "Random.h"

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

Game::Game(Input* const input, IGraphics* graphics) :
	input(input),
	graphics(graphics)
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

	program = new Program();
	vertShader = new Shader(program, Shader::Type::Vertex);
	vertShader->LoadFromFile("../Common/Assets/Shaders/test.vert");
	vertShader->Compile();

	fragShader = new Shader(program, Shader::Type::Fragment);
	fragShader->LoadFromFile("../Common/Assets/Shaders/test.frag");
	fragShader->Compile();

	Shader* shaders[2]{ vertShader, fragShader };
	program->AddShaders<2>(shaders);
	glUseProgram(program->shaderProgram);

	Random::Init();

	// Timing
	auto startTime = std::chrono::system_clock::now();
	auto lastTime = startTime;

	atlas.BindAtlas("../Common/Assets/Textures/atlas.png", GL_REPEAT);

	IMouse& mouse = GetInput().GetMouse();
	mouse.Init();
	
	RendererHelper rendererHelper;
	rendererHelper.Init();

	BlocksDatabase::Init();

	m_world = std::make_unique<World>();
	m_world->SetGenerator(std::make_unique<WorldGen>(m_world.get()));
	m_world->Init(this);

	m_player = std::make_unique<Player>(this);

	graphics->InitGUI();

	while(!quitting)
	{
		graphics->BeginFrame();

		ProcessInput();
		if (input->GetKeyboard().IsKeyPressed(Key::ESCAPE))
		{
			graphics->Quit();
			return;
		}

		auto time = std::chrono::system_clock::now();
		std::chrono::duration<float> delta = time - lastTime;

		gameDeltaTime = delta.count();

		std::chrono::duration<float> elapsed = time - startTime;
		if(elapsed.count() > 0.25f && frameCount > 10)
		{
			m_averageFPS = static_cast<float>(frameCount) / elapsed.count();
			startTime = time;
			frameCount = 0;
			//printf("%f\n", averageFPS);
		}
		
		if (input->GetKeyboard().IsKeyPressed(Key::TAB))
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

		if (input->GetKeyboard().IsKeyPressed(Key::C))
		{
			if (graphics->IsCursorLocked()) graphics->UnlockCursor();
			else graphics->LockCursor();
			GetInput().GetMouse().Init();
		}

		if (input->GetKeyboard().IsKeyPressed(Key::T))
		{
			m_displaySettings = !m_displaySettings;
		}

		ClearScreen();
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		m_player->Update();
		m_world->Update();
		m_world->FixedUpdate();

		m_world->Render(m_player.get());
		m_player->Render();
		DrawCrosshair();
		if (m_displaySettings) DisplaySettings();
		graphics->EndFrame();

		GetInput().GetKeyboard().Update();
		GetInput().GetMouse().Flush();
		graphics->SwapBuffer();

		lastTime = time;
		++frameCount;
		m_fpsDeque.push_back(1.0f / gameDeltaTime);
		if (m_fpsDeque.size() == COLLECT_FPS_DATA_FRAMES)
		{
			m_fpsDeque.pop_front();
		}

		VertexBuffer<FVertex>::NUM_RENDERED = 0;
	}

	graphics->Quit();
}

Input& Game::GetInput()
{
	return *input;
}

void Game::Quit()
{
	quitting = true;
}

void Game::ProcessInput()
{
	Input& input = GetInput();
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
	ImGui::PlotLines("Graph", values.data(), values.size(), 0, "avg", 0.0f, 120.0f);
	ImGui::SameLine();
	ImGui::PushItemWidth(30);
	if (ImGui::DragInt("Num", &COLLECT_FPS_DATA_FRAMES))
	{
		m_fpsDeque.clear();
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	int verts = VertexBuffer<FVertex>::NUM_RENDERED;
	ImGui::DragInt("Vertices", &verts, 0.0f, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::SliderInt("Generation distance", &m_world->GENERATION_DISTANCE, 2, 32);
	ImGui::DragInt("Chunks active", &m_world->CHUNKS_ACTIVE, 0.0f, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::DragInt("Chunks rendered", &m_world->CHUNKS_RENDERED, 0.0f, 0, 0, "%d", ImGuiSliderFlags_NoInput);

	ImGui::SeparatorText("Player");
	ImGui::DragFloat3("Player position", &playerPos.x, 0.0f, 0.0f, 0.0f, "%.1f", ImGuiSliderFlags_NoInput);
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

	glm::vec3 rightHandPos = m_player->m_rightHand.m_transform.GetLocalPosition();
	ImGui::DragFloat3("Right hand", &rightHandPos.x, 0.05f);
	m_player->m_rightHand.m_transform.SetLocalPosition(rightHandPos);

	float val = Random::GetFloat2D(playerBlockPos.x, playerBlockPos.z);
	ImGui::DragFloat("Rand2D", &val, 0.0f, 0.0f, 0.0f, "%.3f", ImGuiSliderFlags_NoInput);

	Block* block = m_world->GetBlockAtWorld(playerBlockPos);
	int light = -1;
	std::string name = "-";
	if (block)
	{
		light = static_cast<int>(block->GetLightLevel());
		name = BlocksDatabase::Get(block->GetID())->name;
	}
	ImGui::DragInt("Light", &light, 0, 0, 0, "%d", ImGuiSliderFlags_NoInput);
	ImGui::Text(name.c_str());
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

void Game::ClearScreen()
{
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
