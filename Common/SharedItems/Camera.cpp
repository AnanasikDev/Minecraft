#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "Input.h"
#include <glh.h>
#include "Game.h"
#include "IInput.h"

float Camera::FOV_MULT = 1.0f;

Camera::Camera(Game* game, const Frustum& frustum, const Transform& transform, float sense) : Gameobject(game, transform), m_frustum(frustum), m_sense(sense)
{
}

Frustum& Camera::GetFrustum()
{
	return m_frustum;
}

glm::mat4 Camera::GetView() const
{
	glm::mat4 res(1.0f);
	res = glm::lookAt(m_transform.GetWorldPosition(), m_transform.GetWorldPosition() + m_transform.GetWorldForward(), m_transform.GetWorldUp());
	return res;
}

glm::mat4 Camera::GetProjection() const
{
	return glm::perspective(glm::radians(m_frustum.m_fov), m_frustum.m_size.x / m_frustum.m_size.y, m_frustum.m_nearPlane, m_frustum.m_farPlane);
}

bool Camera::IsInFrustum(glm::vec3 pos, bool includeVertical, glm::vec3 offset) const
{
	glm::vec3 eye{ m_transform.GetWorldForward() };
	if (!includeVertical) eye.y = 0;

	glm::vec3 dir{ pos - (m_transform.GetWorldPosition() + offset) };
	if (!includeVertical) dir.y = 0;

	eye = glm::normalize(eye);
	dir = glm::normalize(dir);

	constexpr float adj{ 1.25f };
	const float pitchRad	{ glm::abs(glm::radians(m_transform.GetLocalEulerAngles().x)) };
	const float cosPitch	{ glm::max(glm::cos(pitchRad), 0.1f) };
	const float realFov		{ FOV_MULT * adj * m_frustum.m_fov / cosPitch };
	const float threshold	{ glm::cos(glm::radians(realFov / 2.0f)) };

	return glm::dot(eye, dir) > threshold;
}

Frustum::Frustum(float fov, glm::vec2 screen, float near, float far) : m_fov(fov), m_size(screen), m_nearPlane(near), m_farPlane(far)
{
}
