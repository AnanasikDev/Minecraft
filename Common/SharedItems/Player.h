#pragma once

#include <memory>
#include "Gameobject.h"

class Camera;

class Player : public Gameobject
{
public:
	static constexpr float MOUSE_SENSITIVITY = 0.15f;
	static constexpr float BASE_MOVEMENT_SPEED = 0.06f;
	static constexpr float SPRINT_MOVEMENT_SPEED_MULTIPLIER = 8.0f;//1.6f;

public:
	std::unique_ptr<Camera> m_camera;

	Player(Game* game);
	~Player();
	void Update() override;
	void RenderCamera();
};