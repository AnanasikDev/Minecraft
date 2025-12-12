#pragma once

#include <memory>
#include "Gameobject.h"
#include "Vertex.h"
#include "MeshRenderer.h"

class Camera;

template <typename Vertex>
struct BlockCompositeMesh;

class Player : public Gameobject
{
public:
	float m_mouseSensitivity{ 0.15f };
	float m_baseMovementSpeed{ 6.0f };
	float SPRINT_MOVEMENT_SPEED_MULTIPLIER{ 8.0f };//1.6f;
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
};