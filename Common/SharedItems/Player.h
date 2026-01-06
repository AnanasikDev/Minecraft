#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include "Gameobject.h"
#include "Vertex.h"
#include "MeshRenderer.h"
#include "Collider.h"

class Camera;

template <typename Vertex>
struct BlockCompositeMesh;

class Player : public Gameobject
{
public:
	float m_mouseSensitivity{ 0.15f };
	float m_baseMovementSpeed{ 6.0f };
	float SPRINT_MOVEMENT_SPEED_MULTIPLIER{ 1.6f };
	float m_reachDistance{ 60.0f };
	bool  m_rightHandAutoLight{ true };
	float m_jumpPower{ 8.5f };
	Block::ID m_blockSelection{ Block::ID::Lamp };
	MeshRenderer<FVertex> m_rightHand;
	Mesh<FVertex> m_rightHandMesh;
	std::unique_ptr<BlockCompositeMesh<FVertex>> m_compositeMesh;

	std::unique_ptr<Camera> m_camera;

	Collider m_collider;
	bool m_isGrounded{ false };
	bool m_isSprinting{ false };
	glm::vec3 m_velocity;
	glm::vec3 m_input;
	glm::vec3 m_size{ 0.6f, 1.6f, 0.6f };

	Player(Game* game);
	~Player();
	void SetSpawnPosition();
	void Update() override;
	void UpdateInput();
	void UpdateActions();
	void Render();
	void SelectBlock(Block::ID id);

	void FixedUpdate();
	void FindNearbyColliders(const AABB3D& broadphase, std::vector<AABB3D>& outBoxes);
	float ClipAxis(const AABB3D& player, const AABB3D& block, float delta, int axis);
};