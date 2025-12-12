#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "Input.h"
#include <glh.h>
#include "Game.h"
#include "IInput.h"

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

bool Camera::IsInFrustum(glm::vec3 pos, bool includeVertical) const
{	
	glm::vec3 eye{ m_transform.GetWorldForward() };
	if (!includeVertical)
	{
		eye.y = 0;
	}
	glm::vec3 dir{ pos - (m_transform.GetWorldPosition() - eye * 4.0f) };
	if (!includeVertical)
	{
		dir.y = 0;
	}
	eye = glm::normalize(eye);
	dir = glm::normalize(dir);

	const float fovcos = glm::cos(glm::radians(m_frustum.m_fov)) / 2.0f;
	bool isInFrustum{
		1 - glm::dot(eye, dir) < fovcos
	};

	return isInFrustum;
}

Frustum::Frustum(float fov, glm::vec2 screen, float near, float far) : m_fov(fov), m_size(screen), m_nearPlane(near), m_farPlane(far)
{
}

int Frustum::ContainsAaBox(const AABB& refBox) const
{
	//glm::vec3 vCorner[8];
	//int iTotalIn = 0;

	//// get the corners of the box into the vCorner array
	//refBox.GetVertices(vCorner);

	//// test all 8 corners against the 6 sides 
	//// if all points are behind 1 specific plane, we are out
	//// if we are in with all points, then we are fully in
	//for (int p = 0; p < 6; ++p)
	//{

	//	int iInCount = 8;
	//	int iPtIn = 1;

	//	for (int i = 0; i < 8; ++i)
	//	{

	//		// test this point against the planes
	//		if (m_plane[p].SideOfPlane(vCorner[i]) == BEHIND)
	//		{
	//			iPtIn = 0;
	//			--iInCount;
	//		}
	//	}

	//	// were all the points outside of plane p?
	//	If(iInCount == 0)
	//		return(OUT);

	//	// check if they were all on the right side of the plane
	//	iTotalIn += iPtIn;
	//}

	//// so if iTotalIn is 6, then all are inside the view
	//if (iTotalIn == 6)
	//	return(IN);

	//// we must be partly in then otherwise
	//return(INTERSECT);
	return 0;
}
