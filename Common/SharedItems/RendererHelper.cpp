#include "glh.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include "AssetManager.h"
#include "Program.h"
#include "Game.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "RendererHelper.h"
#include "Renderer.h"

void RendererHelper::Init()
{
	static DebugVertex debugVertices[8] = {
		DebugVertex(-0.5f, -0.5f, -0.5f),
		DebugVertex(-0.5f, 0.5f, -0.5f),
		DebugVertex(0.5f, 0.5f, -0.5f),
		DebugVertex(0.5f, -0.5f, -0.5f),
		DebugVertex(-0.5f, -0.5f, 0.5f),
		DebugVertex(-0.5f, 0.5f, 0.5f),
		DebugVertex(0.5f, 0.5f, 0.5f),
		DebugVertex(0.5f, -0.5f, 0.5f),
	};
	static unsigned int debugIndices[12 * 2] = {
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
	};

	Mesh<DebugVertex>::MESH_BOX.AddIndices(debugIndices, 12 * 2);
	Mesh<DebugVertex>::MESH_BOX.AddVertices(debugVertices, 8);

	const glm::ivec3 pos(0, 0, 0);
	const TextureAtlas::TextureID id(TextureAtlas::TextureID::Dirt);
	const unsigned char light( 0xFF );
	Mesh<FVertex>& mesh = Mesh<FVertex>::MESH_BOX;
	VRendererHelper<FVertex>::AddFace(mesh, pos, GridVec::Front,	id, light);
	VRendererHelper<FVertex>::AddFace(mesh, pos, GridVec::Back,		id, light);
	VRendererHelper<FVertex>::AddFace(mesh, pos, GridVec::Top,		id, light);
	VRendererHelper<FVertex>::AddFace(mesh, pos, GridVec::Bottom,	id, light);
	VRendererHelper<FVertex>::AddFace(mesh, pos, GridVec::Right,	id, light);
	VRendererHelper<FVertex>::AddFace(mesh, pos, GridVec::Left,		id, light);

	static SpriteVertex spriteVertices[4] = {
		SpriteVertex(   -0.5f,	-0.5f,		0,				0, 0, 0),
		SpriteVertex(   -0.5f,	 0.5f,		0,				0, 1, 0),
		SpriteVertex(	0.5f,    0.5f,		0,				1, 1, 0),
		SpriteVertex(	0.5f,   -0.5f,		0,				1, 0, 0),
	};
	static unsigned int spriteIndices[6] = {
		0, 2, 1,
		0, 3, 2
	};

	Mesh<SpriteVertex>::MESH_BOX.AddIndices(spriteIndices, 6);
	Mesh<SpriteVertex>::MESH_BOX.AddVertices(spriteVertices, 4);
}

void RendererHelper::Update(Gamerules& gamerules)
{
	if (gamerules.m_doDayNightCycle)
	{
		const float sunIntensity{ GameTime::GetSunIntensity() };
		m_mainProgram->SetFloat("u_SunIntensity", sunIntensity);
		m_waterProgram->SetFloat("u_SunIntensity", sunIntensity);
	}
}

void RendererHelper::DisableDepth()
{
	glDepthFunc(GL_ALWAYS);
}

void RendererHelper::EnableDepth()
{
	glDepthFunc(GL_LEQUAL);
}

void RendererHelper::ClearScreen()
{
	glm::vec3 finalColor = Game::SKY_NIGHT_COLOR + (Game::SKY_DAY_COLOR - Game::SKY_NIGHT_COLOR) * GameTime::GetSunIntensity();
	glClearColor(UnwrapGridVec(finalColor), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererHelper::SetupPrograms(const AssetManager* const assets)
{
	m_mainProgram = std::make_unique<Program>();
	CreateProgram(m_mainProgram.get(), assets->GetAssetPathString("Shaders/test.vert"), assets->GetAssetPathString("Shaders/test.frag"));
	
	m_waterProgram = std::make_unique<Program>();
	CreateProgram(m_waterProgram.get(), assets->GetAssetPathString("Shaders/water.vert"), assets->GetAssetPathString("Shaders/water.frag"));

	m_spriteProgram = std::make_unique<Program>();
	CreateProgram(m_spriteProgram.get(), assets->GetAssetPathString("Shaders/sprite.vert"), assets->GetAssetPathString("Shaders/sprite.frag"));

	m_mainProgram->Use();
}

void RendererHelper::CreateProgram(Program* program, std::string vertPath, std::string fragPath)
{
	Shader* vertShader = new Shader(program, Shader::Type::Vertex);
	vertShader->LoadFromFile(vertPath);
	vertShader->Compile();

	Shader* fragShader = new Shader(program, Shader::Type::Fragment);
	fragShader->LoadFromFile(fragPath);
	fragShader->Compile();

	program->AddShaders(std::vector<Shader*>{vertShader, fragShader});
}
