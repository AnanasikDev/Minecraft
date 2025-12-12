#include "Game.h"
#include "Camera.h"
#include "Player.h"
#include "Input.h"
#include "IInput.h"
#include "Transform.h"
#include "World.h"
#include "BlockCompositeMesh.h"

Player::Player(Game* game) : Gameobject(game)
{
	m_camera = std::make_unique<Camera>(m_game, Frustum(45.0f, glm::vec2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), 0.1f, 1000.0f), Transform(), 0.25f);
	m_camera->m_transform.SetParent(&m_transform);

	SetSpawnPosition();
	
	m_rightHandMesh.CopyFrom(Mesh<FVertex>::MESH_BOX);
	m_rightHandMesh.Modify([](FVertex& v) { v.x -= 0.5f; v.y -= 0.5f; v.z -= 0.5f; });
	m_rightHand.UseMesh(&m_rightHandMesh);
	m_rightHand.UseRendererSystem(m_game->m_renderer.get());
	m_rightHand.m_transform.Translate(glm::vec3(0.3f, -0.3f, -0.75f));
	m_rightHand.m_transform.SetParent(&m_camera->m_transform);
	m_rightHand.m_transform.ScaleLocal(glm::vec3(0.2f));
	m_rightHand.UpdateBuffers();

	//m_transform.Rotate(45, glm::vec3(0, 1, 0));
	m_compositeMesh = std::make_unique<BlockCompositeMesh<FVertex>>();
	m_compositeMesh->m_meshPtr = &m_rightHandMesh;
	SelectBlock(Block::ID::Grass);
}

Player::~Player()
{

}

void Player::SetSpawnPosition()
{
	BlockGenData data{ m_game->m_world->GetGenerator()->GetBlockGenDataAt(glm::ivec3(0, 0, 0)) };
	const int height{ data.GetHeight() };
	m_transform.Translate(glm::vec3(0, height + 3, 0));
}

void Player::Update()
{
	m_rightHand.m_transform.Rotate(1, glm::vec3(0.5f, 0.5f, 0.0f));

	const bool isSprinting{ m_game->GetInput().GetKeyboard().IsKeyDown(Key::SHIFT_LEFT) };

	float currentMovementSpeed{ m_baseMovementSpeed };
	if (isSprinting)
	{
		currentMovementSpeed *= SPRINT_MOVEMENT_SPEED_MULTIPLIER;
	}

	glm::vec3 delta(0, 0, 0);
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::W))
	{
		delta += m_transform.GetLocalForward() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::S))
	{
		delta += -m_transform.GetLocalForward() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::D))
	{
		delta += -m_transform.GetLocalRight() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::A))
	{
		delta += m_transform.GetLocalRight() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::E))
	{
		delta += m_transform.GetLocalUp() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::Q))
	{
		delta += -m_transform.GetLocalUp() * currentMovementSpeed;
	}

	m_transform.Translate(delta * m_game->GetDeltaTime());
	const glm::vec2 mouseDelta{ m_game->GetInput().GetMouse().GetPositionDelta() * m_mouseSensitivity };
	m_transform.Rotate(-mouseDelta.x, glm::vec3(0, 1, 0));
	m_camera->m_transform.Rotate(-mouseDelta.y, glm::vec3(1, 0, 0));

	//if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::Z))
	
	if (m_game->GetInput().GetMouse().IsButtonPressed(MouseButtons::LEFT))
	{
		RaycastResult hit = m_game->m_world->Raycast(Ray(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_camera->m_transform.GetWorldForward() * 60.0f));
		if (hit.m_block != nullptr)
		{
			m_game->m_world->SetAndUpdateBlockAtWorld(hit.m_worldBlockPos + hit.m_normal, m_blockSelection);
		}
	}
	if (m_game->GetInput().GetMouse().IsButtonPressed(MouseButtons::RIGHT))
	{
		RaycastResult hit = m_game->m_world->Raycast(Ray(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_camera->m_transform.GetWorldForward() * 60.0f));
		if (hit.m_block != nullptr)
		{
			m_game->m_world->SetAndUpdateBlockAtWorld(hit.m_worldBlockPos, Block::ID::Air);
		}
	}
	if (m_game->GetInput().GetMouse().IsButtonPressed(MouseButtons::MIDDLE))
	{
		RaycastResult hit = m_game->m_world->Raycast(Ray(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_camera->m_transform.GetWorldForward() * 60.0f));
		if (hit.m_block != nullptr)
		{
			m_game->m_world->SetAndUpdateBlockAtWorld(hit.m_worldBlockPos, m_blockSelection);
		}
	}

	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_1))
	{
		SelectBlock(Block::ID::Dirt);
	}
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_2))
	{
		SelectBlock(Block::ID::Grass);
	}
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_3))
	{
		SelectBlock(Block::ID::Lamp);
	}
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_4))
	{
		SelectBlock(Block::ID::Stone);
	}
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_5))
	{
		SelectBlock(Block::ID::OakLeaves);
	}
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_6))
	{
		SelectBlock(Block::ID::OakLog);
	}
}

void Player::Render()
{
	glDepthFunc(GL_ALWAYS); // TODO: abstract away
	m_rightHand.Render(m_camera.get());
	glDepthFunc(GL_LEQUAL);
}

void Player::SelectBlock(Block::ID id)
{
	m_blockSelection = id;
	m_rightHandMesh.ClearGeometry();

	for (GridVec& v : directions)
	{
		GeomContext ctx(glm::ivec3(0), glm::ivec3(0), v, nullptr, nullptr, m_compositeMesh.get());
		BlocksDatabase::Get(id)->GenerateGeometry(ctx);
	}
	m_rightHandMesh.Modify([](FVertex& v) { v.x -= 0.5f; v.y -= 0.5f; v.z -= 0.5f; });
	m_rightHand.UpdateBuffers();
}
