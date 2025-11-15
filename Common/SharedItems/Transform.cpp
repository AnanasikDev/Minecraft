#include "Transform.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/common.hpp"
#include <algorithm>

Transform::~Transform()
{
	AttachTo(nullptr);
	// TODO: move desruction to Gameobject - Transform is not responsible for deleting objects
	std::vector<Transform*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); ++it)
	{
		(*it)->AttachTo(nullptr);
	}
}

void Transform::Translate(glm::vec3 delta)
{
	m_position += delta;
	std::vector<Transform*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); ++it)
	{
		(*it)->Translate(delta);
	}
}

void Transform::Warp(glm::vec3 target)
{
	Translate(target - m_position);
}

void Transform::Rotate(float degrees, glm::vec3 axis)
{
	glm::mat4 mat(1.0f);
	const float radians = glm::radians(degrees);
	mat = glm::rotate(mat, radians, axis);
	m_forward	= mat * glm::vec4(m_forward, 0.0f);
	m_right		= mat * glm::vec4(m_right, 0.0f);
	m_up		= mat * glm::vec4(m_up, 0.0f);
	glm::quat delta = glm::angleAxis(radians, axis);
	m_quat = delta * m_quat;

	std::vector<Transform*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); ++it)
	{
		(*it)->Rotate(degrees, axis);
	}

	// TODO: relative rotations for children
}

void Transform::Scale(glm::vec3 scale)
{
	m_scale *= scale;
	// TODO: relative scale for children
}

void Transform::AttachTo(Transform* parent)
{
	if (m_parent)
	{
		m_parent->m_children.erase(std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this));
		m_parent = nullptr;
	}

	m_parent = parent;
	if (parent == nullptr)
	{
		return;
	}

	parent->m_children.push_back(this);
}

glm::mat4 Transform::GetModelMatrix() const
{
	glm::mat4 mat(1.0f);
	mat = glm::scale(mat, m_scale);
	glm::mat4 rot = glm::toMat4(m_quat);
	mat = rot * mat;
	mat = glm::translate(mat, m_position);
	return mat;
}
