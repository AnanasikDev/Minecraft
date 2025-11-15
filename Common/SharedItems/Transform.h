#pragma once

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include "glm/gtx/quaternion.hpp"

#include <vector>

class Transform
{
private:
	glm::vec3 m_position{ 0,0,0 };
	glm::quat m_quat{};
	glm::vec3 m_scale{ 0,0,0 };
	glm::vec3 m_forward{ 0,0,1 };
	glm::vec3 m_up{ 0,1,0 };
	glm::vec3 m_right{ 1,0,0 };

public:

	Transform* m_parent;
	std::vector<Transform*> m_children;

	Transform() = default;

	Transform(glm::vec3 position)
	{
		Translate(position);
	}
	~Transform();

	void Translate(glm::vec3 delta);
	void Warp(glm::vec3 target);
	void Rotate(float degrees, glm::vec3 axis);
	void Scale(glm::vec3 scale);

	void AttachTo(Transform* parent);

	glm::mat4 GetModelMatrix() const;

public:
	inline glm::vec3 GetPosition() const { return m_position; }
	inline glm::vec3 GetScale() const { return m_scale; }
	inline glm::quat GetRotation() const { return m_quat; }
	inline glm::vec3 GetForward() const { return m_forward; }
	inline glm::vec3 GetUp() const { return m_up; }
	inline glm::vec3 GetRight() const { return m_right; }
};
