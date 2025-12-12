#include "Transform.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <algorithm>

Transform::Transform() {}

Transform::~Transform()
{
    for (auto child : m_children)
    {
        child->m_parent = nullptr;
        child->MarkDirty();
    }
    if (m_parent)
    {
        std::vector<Transform*>& pChildren = m_parent->m_children;
        pChildren.erase(std::remove(pChildren.begin(), pChildren.end(), this), pChildren.end());
    }
}

void Transform::DecomposeWorldMatrix(glm::vec3& scale, glm::quat& rotation, glm::vec3& position) const
{
    const glm::mat4& worldMatrix = (const_cast<Transform*>(this))->GetWorldMatrix();

    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(worldMatrix, scale, rotation, position, skew, perspective);
}

void Transform::SetParent(Transform* parent)
{
    if (m_parent)
    {
        std::vector<Transform*>& pChildren = m_parent->m_children;
        pChildren.erase(std::remove(pChildren.begin(), pChildren.end(), this), pChildren.end());
    }

    m_parent = parent;

    if (m_parent)
    {
        m_parent->m_children.push_back(this);
    }

    MarkDirty();
}

void Transform::MarkDirty()
{
    if (m_isWorldDirty) return;

    m_isWorldDirty = true;

    for (auto child : m_children)
    {
        child->MarkDirty();
    }
}

void Transform::SetLocalPosition(const glm::vec3& pos)
{
    m_localPosition = pos;
    m_isLocalDirty = true;
    MarkDirty();          
}

void Transform::SetLocalRotation(const glm::quat& rot)
{
    m_localRotation = rot;
    m_isLocalDirty = true;
    MarkDirty();
}

void Transform::SetLocalScale(const glm::vec3& scale)
{
    m_localScale = scale;
    m_isLocalDirty = true;
    MarkDirty();
}

void Transform::ScaleLocal(const glm::vec3& factor)
{
    SetLocalScale(m_localScale * factor);
}

void Transform::Translate(const glm::vec3& delta)
{
    SetLocalPosition(m_localPosition + delta);
}

void Transform::Rotate(const glm::quat& delta)
{
    SetLocalRotation(delta * m_localRotation);
}

void Transform::Rotate(float degrees, glm::vec3 axis)
{
    SetLocalRotation(m_localRotation * glm::angleAxis(glm::radians(degrees), glm::normalize(axis)));
}

const glm::mat4& Transform::GetLocalMatrix() const
{
    if (m_isLocalDirty)
    {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), m_localPosition);
        glm::mat4 r = glm::toMat4(m_localRotation);
        glm::mat4 s = glm::scale(glm::mat4(1.0f), m_localScale);
        
        m_localMatrix = t * r * s;
        m_isLocalDirty = false;
    }
    return m_localMatrix;
}

const glm::mat4& Transform::GetWorldMatrix() const
{
    if (m_isWorldDirty)
    {
        if (m_parent)
        {
            m_worldMatrix = m_parent->GetWorldMatrix() * GetLocalMatrix();
        }
        else
        {
            m_worldMatrix = GetLocalMatrix();
        }
        m_isWorldDirty = false;
    }
    return m_worldMatrix;
}

glm::vec3 Transform::GetLocalPosition() const
{
    return m_localPosition;
}

glm::quat Transform::GetLocalRotation() const
{
    return m_localRotation;
}

glm::vec3 Transform::GetLocalScale() const
{
    return m_localScale;
}

glm::vec3 Transform::GetWorldPosition() const
{
    return GetWorldMatrix()[3];
}

glm::quat Transform::GetWorldRotation() const
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;
    DecomposeWorldMatrix(scale, rotation, position);
    return rotation;
}

glm::vec3 Transform::GetWorldScale() const
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;
    DecomposeWorldMatrix(scale, rotation, position);
    return scale;
}

glm::vec3 Transform::GetLocalForward() const
{
    return glm::rotate(m_localRotation, glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Transform::GetLocalRight() const
{
    return glm::rotate(m_localRotation, glm::vec3(-1.0f, 0.0f, 0.0f));
}

glm::vec3 Transform::GetLocalUp() const
{
    return glm::rotate(m_localRotation, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Transform::GetWorldForward() const
{
    glm::quat worldRotation = GetWorldRotation();
    return glm::rotate(worldRotation, glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Transform::GetWorldRight() const
{
    glm::quat worldRotation = GetWorldRotation();
    return glm::rotate(worldRotation, glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Transform::GetWorldUp() const
{
    glm::quat worldRotation = GetWorldRotation();
    return glm::rotate(worldRotation, glm::vec3(0.0f, 1.0f, 0.0f));
}
