#pragma once

#include "MeshRenderer.h"
#include "Camera.h"

template <typename Vertex>
RenderRequest<Vertex>::RenderRequest(MeshRenderer<Vertex>* const meshRenderer, const Camera* const camera)
{
	m_matModel = meshRenderer->m_transform.GetModelMatrix();
	m_modeOverride = meshRenderer->m_mode;
	m_matView = camera->GetView();
	m_matProjection = camera->GetProjection();
	m_vbo = &meshRenderer->m_vbo;
	m_ebo = &meshRenderer->m_ebo;
	m_vao = meshRenderer->m_vaoid;
}