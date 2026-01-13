#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include "Gamerules.h"
#include "Gameobject.h"
#include "Vertex.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "Health.h"

class Camera;

template <typename Vertex>
struct BlockCompositeMesh;

class Player : public Gameobject
{
public:
	float m_mouseSensitivity{ 0.15f };
	float m_baseMovementSpeed{ 6.0f };
	static constexpr float SPRINT_MOVEMENT_SPEED_MULTIPLIER{ 1.6f };
	static constexpr float CROUCH_MOVEMENT_SPEED_MULTIPLIER{ 0.65f };
	static constexpr float FLIGHT_MOVEMENT_SPEED_MULTIPLIER{ 6.0f };
	static constexpr float SPRINT_POV_MULTIPLIER{ 1.18f };
	float m_reachDistance{ 60.0f };
	bool  m_rightHandAutoLight{ true };
	float m_jumpPower{ 8.5f };
	Gamerules::PhysicsMode m_mode{ Gamerules::PhysicsMode::Flight };

	Block::ID m_blockSelection{ Block::ID::Lamp };
	MeshRenderer<FVertex> m_rightHand;
	Mesh<FVertex> m_rightHandMesh;
	std::unique_ptr<BlockCompositeMesh<FVertex>> m_compositeMesh;
	std::unique_ptr<Camera> m_camera;

	Player(Game* game);
	~Player();
	void SetSpawnPosition();
	void Update() override;
	void Render();
	void SelectBlock(Block::ID id);

	float GetHeight() const;

private:
	Collider m_collider;
	bool m_isGrounded{ false };
	bool m_isSprinting{ false };
	bool m_isCrouching{ false };
	glm::vec3 m_velocity;
	glm::vec3 m_input;
	glm::vec2 m_mouseDelta;
	float m_normalHeight{ 1.79f };
	float m_crouchHeight{ 0.85f };
	float m_xySize{ 0.6f };
	float m_headEyeLevel{ 0.15f };
	Timer m_toggleFlightTimer;
	float m_toggleFlightDelay{ 0.2f };
	Health m_health;

	void MoveFlight();
	void MovePhysics();
	void FindNearbyColliders(const AABB3D& broadphase, std::vector<AABB3D>& outBoxes);
	float ClipAxis(const AABB3D& player, const AABB3D& block, float delta, int axis);
	float GetCurrentMovementSpeed() const;

	void UpdateInput();
	void UpdateActions();
	void SetHeight(float height);
	void OnFall();
};
