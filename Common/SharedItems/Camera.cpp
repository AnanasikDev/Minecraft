#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "Input.h"
#include <glh.h>
#include "Game.h"
#include "IInput.h"

Camera::Camera(Game* game, const Frustum& frustum, const Transform& transform, float speed, float sense)
{
	m_game = game;
	m_frustum = frustum;
	m_transform = transform;
	m_speed = speed;
	m_sense = sense;
}

glm::mat4 Camera::GetView() const
{
	glm::mat4 res(1.0f);
	res = glm::lookAt(m_transform.GetPosition(), m_transform.GetPosition() + m_transform.GetForward(), m_transform.GetUp());
	res = glm::translate(res, -m_transform.GetPosition());
	return res;
}

glm::mat4 Camera::GetProjection() const
{
	return glm::perspective(glm::radians(m_frustum.m_fov), m_frustum.m_size.x / m_frustum.m_size.y, m_frustum.m_nearPlane, m_frustum.m_farPlane);
}

Frustum::Frustum(float fov, glm::vec2 screen, float near, float far)
{
	m_fov = fov;
	m_size = screen;
	m_nearPlane = near;
	m_farPlane = far;
}
