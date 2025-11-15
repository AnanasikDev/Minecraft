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

Game::Game(const Input* const input, IGraphics* graphics) :
	input(input),
	graphics(graphics)
{

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
	vertShader = new Shader(program, Shader::Type::FVertex);
	vertShader->LoadFromFile("../Common/SharedItems/test.vert");
	vertShader->Compile();

	fragShader = new Shader(program, Shader::Type::Fragment);
	fragShader->LoadFromFile("../Common/SharedItems/test.frag");
	fragShader->Compile();

	Shader* shaders[2]{ vertShader, fragShader };
	program->AddShaders<2>(shaders);
	glUseProgram(program->shaderProgram);

	Random::Init();

	// Timing
	auto startTime = std::chrono::system_clock::now();
	auto lastTime = startTime;

	float averageFPS{ 0 };
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	atlas.ReadAtlas("../Common/Assets/Textures/atlas.png", GL_REPEAT);

	const IMouse& mouse = GetInput().GetMouse();
	mouse.Init();

	m_player = std::make_unique<Player>(this);

	blocks = new BlocksDatabase();
	blocks->Init();

	m_world = std::make_unique<World>();
	m_world->Init(this);

	while(!quitting)
	{
		ProcessInput();
		if (input->GetKeyboard().GetKey(Key::ESCAPE))
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
		// Setup the viewport
		ClearScreen();
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		//Update and Draw your game here
		//Don't forget to use gameDeltaTime for smooth movement

		m_player->Update();
		m_world->Update();

		m_world->Render(m_player.get());

		glFlush();
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

	glCullFace(GL_BACK);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Game::ClearScreen()
{
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
