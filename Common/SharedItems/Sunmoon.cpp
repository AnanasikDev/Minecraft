#include "commons.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "AssetManager.h"
#include "Game.h"
#include "TextureAtlas.h"
#include "Sunmoon.h"
#include "Renderer.h"
#include "RendererHelper.h"
#include "Player.h"

void Sunmoon::Init(Game* game)
{
	m_game = game;
	m_atlas = std::make_unique<TextureAtlas>();
	m_atlas->Create3D(m_game->m_assetManager->GetAssetPathString("Textures/sunmoon.png"), GL_REPEAT, 32);

	m_axis = glm::normalize(m_axis);
	m_baseDirection = glm::normalize(m_baseDirection);

	m_sun.UseRendererSystem(m_game->m_renderer.get());
	m_sun.UseProgram(game->m_renderer->m_helper->m_spriteProgram.get());
	m_sun.UseMesh(&Mesh<SpriteVertex>::MESH_BOX);
	m_sun.m_transform.ScaleLocal(glm::vec3(SIZE));
	m_sun.UpdateBuffers();

	m_moon.UseRendererSystem(m_game->m_renderer.get());
	m_moon.UseProgram(game->m_renderer->m_helper->m_spriteProgram.get());
	m_moonMesh.CopyFrom(Mesh<SpriteVertex>::MESH_BOX);
	m_moonMesh.Modify([](SpriteVertex& v) { v.texid = 1; });
	m_moon.UseMesh(&m_moonMesh);
	m_moon.m_transform.ScaleLocal(glm::vec3(SIZE));
	m_moon.UpdateBuffers();

	Move();
}

void Sunmoon::Update(const Player* const player)
{
	Move();
}

void Sunmoon::Render(const Player* const player)
{
	m_atlas->Use();
	m_sun.Render(player->m_camera.get());
	m_moon.Render(player->m_camera.get());
}

void Sunmoon::Move()
{
	glm::quat rotationQuat = glm::angleAxis(-glm::two_pi<float>() * GameTime::TimeOfDayUnclamped(), m_axis);
	glm::vec3 sunDirection = rotationQuat * m_baseDirection;
	glm::vec3 moonDirection = rotationQuat * -m_baseDirection;

	glm::vec3 playerWorldPos = m_game->m_player->m_transform.GetWorldPosition();
	glm::vec3 sunTargetPos = playerWorldPos - sunDirection * DISTANCE;
	glm::vec3 moonTargetPos = playerWorldPos - moonDirection * DISTANCE;

	m_sun.m_transform.SetLocalPosition(sunTargetPos);
	m_sun.m_transform.LookAt(playerWorldPos, glm::vec3(0, 1, 0));

	m_moon.m_transform.SetLocalPosition(moonTargetPos);
	m_moon.m_transform.LookAt(playerWorldPos, glm::vec3(0, 1, 0));
}