#pragma once

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "Transform.h"
#include "Gameobject.h"

struct Frustum
{
	float m_fov;
	glm::vec2 m_size;
	float m_nearPlane;
	float m_farPlane;

	Frustum() = default;
	Frustum(float fov, glm::vec2 screen, float near, float far);
};

class Camera : public Gameobject
{
private:

	Frustum m_frustum;
	float m_sense;

public:
	Camera() = default;
	Camera(Game* game, const Frustum& frustum, const Transform& transform, float sense);

	glm::mat4 GetView() const;
	glm::mat4 GetProjection() const;
};
