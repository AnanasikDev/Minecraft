#pragma once

#include <memory>
#include "glm/vec3.hpp"
#include "Mesh.h"
#include "Vertex.h"

class TextureAtlas;
class Game;

class Sunmoon
{
public:
	Sunmoon() = default;
	~Sunmoon() = default;

	void Init(Game* game);
	void Update(const Player* const player);
	void Render(const Player* const player);
	void Move();

private:
	std::unique_ptr<TextureAtlas> m_atlas;
	Game* m_game{ nullptr };
	MeshRenderer<SpriteVertex> m_sun;
	Mesh<SpriteVertex> m_moonMesh;
	MeshRenderer<SpriteVertex> m_moon;
	glm::vec3 m_baseDirection{ 1, 0, 1 };
	glm::vec3 m_axis{ -1, 0, 1 };
	constexpr static float DISTANCE{ 250.0f };
	constexpr static float SIZE{ 15.0f };
};