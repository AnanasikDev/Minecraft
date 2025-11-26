#include "Game.h"
#include "Camera.h"
#include "Player.h"
#include "Input.h"
#include "IInput.h"
#include "Transform.h"
#include "World.h"

Player::Player(Game* game) : Gameobject(game)
{
	m_camera = std::make_unique<Camera>(m_game, Frustum(45.0f, glm::vec2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT), 0.1f, 1000.0f), Transform(), 0.25f);
	m_camera->m_transform.AttachTo(&m_transform);
	m_transform.Translate(glm::vec3(0, 25, 0));
	m_transform.Rotate(45, glm::vec3(0, 1, 0));
}

Player::~Player()
{

}

void Player::Update()
{
	const bool isSprinting{ m_game->GetInput().GetKeyboard().IsKeyDown(Key::SHIFT_LEFT) };

	float currentMovementSpeed{ BASE_MOVEMENT_SPEED };
	if (isSprinting)
	{
		currentMovementSpeed *= SPRINT_MOVEMENT_SPEED_MULTIPLIER;
	}

	glm::vec3 delta(0, 0, 0);
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::W))
	{
		delta += m_transform.GetForward() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::S))
	{
		delta += -m_transform.GetForward() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::D))
	{
		delta += -m_transform.GetRight() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::A))
	{
		delta += m_transform.GetRight() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::E))
	{
		delta += m_transform.GetUp() * currentMovementSpeed;
	}
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::Q))
	{
		delta += -m_transform.GetUp() * currentMovementSpeed;
	}

	m_transform.Translate(delta);
	m_transform.Rotate(m_game->GetInput().GetMouse().GetPositionDelta().x * -MOUSE_SENSITIVITY, glm::vec3(0, 1, 0));
	m_camera->m_transform.Rotate(m_game->GetInput().GetMouse().GetPositionDelta().y * MOUSE_SENSITIVITY, m_transform.GetRight());

	if (m_game->GetInput().GetMouse().GetButtonDown(MouseButtons::LEFT))
	{
	}
}

void Player::RenderCamera()
{

}