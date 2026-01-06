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

	m_rightHand.UseMesh(&m_rightHandMesh);
	m_rightHand.UseRendererSystem(m_game->m_renderer.get());
	m_rightHand.m_transform.Translate(glm::vec3(0.3f, -0.3f, -0.75f));
	m_rightHand.m_transform.SetParent(&m_camera->m_transform);
	m_rightHand.m_transform.ScaleLocal(glm::vec3(0.2f));
	m_rightHand.UseProgram(m_game->m_renderer->m_helper->m_mainProgram.get());

	m_compositeMesh = std::make_unique<BlockCompositeMesh<FVertex>>();
	m_compositeMesh->m_meshPtr = &m_rightHandMesh;
	SelectBlock(Block::ID::Grass);

	m_collider.AddBox(glm::vec3(-m_size.x / 2.0f, -m_size.y, -m_size.x / 2.0f), glm::vec3(m_size.x / 2.0f, 0.0f, m_size.x / 2.0f));
}

Player::~Player() {}

void Player::SetSpawnPosition()
{
	BlockGenData data{ m_game->m_world->GetGenerator()->GetBlockGenDataAt(glm::ivec3(0, 0, 0)) };
	const int height{ data.GetHeight() };
	m_transform.SetLocalPosition(glm::vec3(0, height + 3, 0));
}

void Player::UpdateInput()
{
	m_isSprinting = m_game->GetInput().GetKeyboard().IsKeyDown(Key::SHIFT_LEFT);

	float currentMovementSpeed{ m_baseMovementSpeed };
	if (m_isSprinting)
	{
		currentMovementSpeed *= SPRINT_MOVEMENT_SPEED_MULTIPLIER;
	}

	glm::vec3 moveDir(0, 0, 0);
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::W)) moveDir += m_transform.GetLocalForward();
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::S)) moveDir -= m_transform.GetLocalForward();
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::D)) moveDir -= m_transform.GetLocalRight();
	if (m_game->GetInput().GetKeyboard().IsKeyDown(Key::A)) moveDir += m_transform.GetLocalRight();

	// Flatten movement
	moveDir.y = 0;
	if (glm::length(moveDir) > 0)
	{
		moveDir = glm::normalize(moveDir);
	}

	m_input = moveDir * currentMovementSpeed;

	// Jump
	if (m_isGrounded && m_game->GetInput().GetKeyboard().IsKeyDown(Key::SPACE))
	{
		m_velocity.y = m_jumpPower;
		m_isGrounded = false;
	}
}

void Player::UpdateActions()
{
	if (m_game->GetInput().GetMouse().IsButtonPressed(MouseButtons::LEFT))
	{
		RaycastResult hit = m_game->m_world->Raycast(Ray(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_camera->m_transform.GetWorldForward() * m_reachDistance));
		if (hit.m_block != nullptr)
		{
			m_game->m_world->SetAndUpdateBlockAtWorld(hit.m_worldBlockPos + hit.m_normal, m_blockSelection);
		}
	}
	if (m_game->GetInput().GetMouse().IsButtonPressed(MouseButtons::RIGHT))
	{
		RaycastResult hit = m_game->m_world->Raycast(Ray(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_camera->m_transform.GetWorldForward() * m_reachDistance));
		if (hit.m_block != nullptr)
		{
			m_game->m_world->SetAndUpdateBlockAtWorld(hit.m_worldBlockPos, Block::ID::Air);
		}
	}
	if (m_game->GetInput().GetMouse().IsButtonPressed(MouseButtons::MIDDLE))
	{
		RaycastResult hit = m_game->m_world->Raycast(Ray(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_camera->m_transform.GetWorldForward() * m_reachDistance));
		if (hit.m_block != nullptr)
		{
			SelectBlock(hit.m_block->GetID());
		}
	}
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_1)) SelectBlock(Block::ID::Dirt);
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_2)) SelectBlock(Block::ID::Grass);
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_3)) SelectBlock(Block::ID::Lamp);
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_4)) SelectBlock(Block::ID::Stone);
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_5)) SelectBlock(Block::ID::OakLeaves);
	if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::NUM_6)) SelectBlock(Block::ID::OakLog);
}

void Player::Update()
{
	m_rightHand.m_transform.Rotate(1, glm::vec3(0.5f, 0.5f, 0.0f));

	UpdateInput();

	const glm::vec2 mouseDelta{ m_game->GetInput().GetMouse().GetPositionDelta() * m_mouseSensitivity };
	m_transform.Rotate(-mouseDelta.x, glm::vec3(0, 1, 0));
	m_camera->m_transform.Rotate(-mouseDelta.y, glm::vec3(1, 0, 0));

	UpdateActions();

	FixedUpdate();
}

void Player::FixedUpdate()
{
	float dt = m_game->GetDeltaTime();

	m_velocity.x = m_input.x;
	m_velocity.z = m_input.z;

	m_velocity.y -= m_game->GetGamerules().m_Gravity * dt;

	glm::vec3 movement = m_velocity * dt;

	AABB3D playerBox = m_collider.m_boxes[0];
	playerBox.m_min += m_transform.GetWorldPosition();
	playerBox.m_max += m_transform.GetWorldPosition();

	AABB3D sweep = playerBox;
	sweep.m_min += glm::min(movement, glm::vec3(0));
	sweep.m_max += glm::max(movement, glm::vec3(0));

	std::vector<AABB3D> candidateBlocks;
	FindNearbyColliders(sweep, candidateBlocks);

	glm::vec3 finalMovement(0.0f);

	// Y Axis
	for (const auto& blockBox : candidateBlocks)
	{
		movement.y = ClipAxis(playerBox, blockBox, movement.y, 1);
	}
	playerBox.m_min.y += movement.y;
	playerBox.m_max.y += movement.y;
	finalMovement.y = movement.y;

	// X Axis
	for (const auto& blockBox : candidateBlocks)
	{
		movement.x = ClipAxis(playerBox, blockBox, movement.x, 0);
	}
	playerBox.m_min.x += movement.x;
	playerBox.m_max.x += movement.x;
	finalMovement.x = movement.x;

	// Z Axis
	for (const auto& blockBox : candidateBlocks)
	{
		movement.z = ClipAxis(playerBox, blockBox, movement.z, 2);
	}
	playerBox.m_min.z += movement.z;
	playerBox.m_max.z += movement.z;
	finalMovement.z = movement.z;

	m_transform.Translate(finalMovement);
	if (std::abs(movement.y - (m_velocity.y * dt)) > 0.0001f)
	{
		if (m_velocity.y < 0) m_isGrounded = true;
		m_velocity.y = 0;
	}
	else
	{
		m_isGrounded = false;
	}
}

void Player::FindNearbyColliders(const AABB3D& broadphase, std::vector<AABB3D>& outBoxes)
{
	// Expand search area slightly to catch nearby boxes
	constexpr float eps{ 0.1f };
	glm::ivec3 minB = World::SnapToBlock(broadphase.m_min - eps);
	glm::ivec3 maxB = World::SnapToBlock(broadphase.m_max + eps);

	for (int x = minB.x; x <= maxB.x; ++x)
	{
		for (int y = minB.y; y <= maxB.y; ++y)
		{
			for (int z = minB.z; z <= maxB.z; ++z)
			{
				const Block* const block{ m_game->m_world->GetBlockAtWorld({ x, y, z }) };
				if (block && block->GetID() != Block::ID::Air)
				{
					BlockData* data{ block->GetData() };
					if (data && data->IsSolid())
					{
						for (auto box : data->m_collider.m_boxes)
						{
							box.m_min += glm::vec3(x, y, z);
							box.m_max += glm::vec3(x, y, z);
							outBoxes.push_back(box);
						}
					}
				}
			}
		}
	}
}

float Player::ClipAxis(const AABB3D& player, const AABB3D& block, float delta, int axis)
{
	const int a1{ (axis + 1) % 3 };
	const int a2{ (axis + 2) % 3 };

	if (player.m_max[a1] > block.m_min[a1] && player.m_min[a1] < block.m_max[a1])
	{
		if (player.m_max[a2] > block.m_min[a2] && player.m_min[a2] < block.m_max[a2])
		{
			if (delta > 0.0f && player.m_max[axis] <= block.m_min[axis])
			{
				float maxAllowed{ block.m_min[axis] - player.m_max[axis] };
				if (maxAllowed < delta) delta = maxAllowed;
			}
			else if (delta < 0.0f && player.m_min[axis] >= block.m_max[axis])
			{
				float maxAllowed{ block.m_max[axis] - player.m_min[axis] };
				if (maxAllowed > delta) delta = maxAllowed;
			}
		}
	}
	return delta;
}

void Player::Render()
{
	m_game->m_renderer->Get()->DisableDepth();
	m_rightHand.Render(m_camera.get());
	m_game->m_renderer->Get()->EnableDepth();
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
	m_rightHandMesh.Modify([&](FVertex& v)
		{
			v.x -= 0.5f; v.y -= 0.5f; v.z -= 0.5f;
			if (m_rightHandAutoLight) v.m_light = 0xFF;
		});
	m_rightHand.UpdateBuffers();
}