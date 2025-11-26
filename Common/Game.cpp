#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

Game::Game(const Input* const input, IGraphics* graphics) :
	input(input),
	graphics(graphics)
{
	m_renderer = std::make_unique<Renderer>();
	m_renderer->m_game = this;
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

	float averageFPS{ 0 };
	

	atlas.BindAtlas("../Common/Assets/Textures/atlas.png", GL_REPEAT);

	const IMouse& mouse = GetInput().GetMouse();
	mouse.Init();

	m_player = std::make_unique<Player>(this);

	blocks = new BlocksDatabase();
	blocks->Init();

	m_world = std::make_unique<World>();
	m_world->Init(this);

	static float vertices[8 * 3] = {
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f,
	};
	/*static unsigned int indices[12 * 2] = {
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};*/

	/*static unsigned int indices[4] = {
		0, 1, 2, 3
	};

	Mesh<DebugVertex>::MESH_BOX.AddVertices(reinterpret_cast<DebugVertex*>(vertices), 8 * 3);
	Mesh<DebugVertex>::MESH_BOX.AddIndices(indices, 4);

	MeshRenderer<DebugVertex> meshRenderer;
	meshRenderer.UseMesh(&Mesh<DebugVertex>::MESH_BOX);
	meshRenderer.UseRendererSystem(m_renderer.get());
	meshRenderer.UpdateBuffers();
	meshRenderer.m_mode = RENDER_MODE::LINE_MODE;
	meshRenderer.m_transform.Scale(glm::vec3(16, 20, 16));*/

	while(!quitting)
	{
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
			averageFPS = static_cast<float>(frameCount) / elapsed.count();
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

		// Setup the viewport
		ClearScreen();
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		//trans.Rotate(0.1f, glm::vec3(0.0f, 1.0f, 0.0));

		//Update and Draw your game here
		//Don't forget to use gameDeltaTime for smooth movement

		m_player->Update();
		m_world->Update();
		m_world->FixedUpdate();

		m_world->Render(m_player.get());

		glm::ivec3 playerChunkId = m_player->m_transform.GetChunkPosition();
		Chunk* chunk = m_world->GetChunkAt(playerChunkId);
		if (chunk)
		{
			glm::ivec3 world = m_player->m_transform.GetBlockPosition();
			glm::ivec3 local = chunk->WorldToLocal(world);
			int height = m_world->GetHeightAt(world);
			Block::ID expectedblockid = m_world->GetBlockIDAt(world);
			if (local.y >= Chunk::YHEIGHT) local.y = Chunk::YHEIGHT - 1;
			Block::ID realblockid = chunk->At(local)->m_data->id;
			//printf("chunk: %02d %02d  local: %02d %02d %02d  world: %02d %02d %02d | expected block: %d  real block: %d\n", playerChunkId.x, playerChunkId.z, local.x, local.y, local.z, world.x, world.y, world.z, expectedblockid, realblockid);
		}
		//meshRenderer.Render(m_player->m_camera.get());

		/*meshRenderer.m_transform.Rotate(10, glm::vec3(0.5f, 0.5f, 0.0));
		meshRenderer.Render(m_player->m_camera.get());*/

		glFlush();
		GetInput().GetKeyboard().Update();
		graphics->SwapBuffer();

		lastTime = time;
		++frameCount;
	}

	graphics->Quit();
}

const Input& Game::GetInput() const
{
	return *input;
}

void Game::Quit()
{
	quitting = true;
}


//example of using the key and mouse
void Game::ProcessInput()
{
	const Input& input = GetInput();
	const IMouse& mouse = GetInput().GetMouse();

	mouse.Update();
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
