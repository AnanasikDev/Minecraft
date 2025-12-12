#pragma once
#include <vector>
#include <glm/glm.hpp>

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include "glm/gtx/quaternion.hpp"

class Transform
{
public:
    Transform();
    ~Transform();

    void SetParent(Transform* parent);
    const std::vector<Transform*>& GetChildren() const { return m_children; }
    void DecomposeWorldMatrix(glm::vec3& scale, glm::quat& rotation, glm::vec3& position) const;

    void SetLocalPosition(const glm::vec3& pos);
    void SetLocalRotation(const glm::quat& rot);
    void SetLocalScale(const glm::vec3& scale);
    void ScaleLocal(const glm::vec3& factor);

    void Translate(const glm::vec3& delta);
    void Rotate(const glm::quat& delta);
    void Rotate(float degrees, glm::vec3 axis);

    const glm::mat4& GetLocalMatrix() const;
    const glm::mat4& GetWorldMatrix() const;

    glm::vec3 GetLocalPosition() const;
    glm::quat GetLocalRotation() const;
    glm::vec3 GetLocalScale() const;

    glm::vec3 GetWorldPosition() const;
    glm::quat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;

    glm::vec3 GetLocalForward() const;
    glm::vec3 GetLocalRight() const;
    glm::vec3 GetLocalUp() const;

    glm::vec3 GetWorldForward() const;
    glm::vec3 GetWorldRight() const;
    glm::vec3 GetWorldUp() const;

private:
    void MarkDirty();

    Transform* m_parent = nullptr;
    std::vector<Transform*> m_children;

    glm::vec3 m_localPosition = glm::vec3(0.0f);
    glm::quat m_localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 m_localScale = glm::vec3(1.0f);

    mutable glm::mat4 m_localMatrix = glm::mat4(1.0f);
    mutable glm::mat4 m_worldMatrix = glm::mat4(1.0f);

    mutable bool m_isLocalDirty = true;
    mutable bool m_isWorldDirty = true;
};