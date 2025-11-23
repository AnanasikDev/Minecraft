#pragma once

#include <memory>
#include "Gameobject.h"

class Camera;

class Player : public Gameobject
{
public:
	static constexpr float MOUSE_SENSITIVITY = 0.15f;
	static constexpr float MOVEMENT_SPEED = 0.3f;

public:
	std::unique_ptr<Camera> m_camera;

	Player(Game* game);
	~Player();
	void Update() override;
	void RenderCamera();
};