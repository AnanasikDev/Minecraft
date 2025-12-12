#pragma once

#include "glh.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include "Program.h"
#include "Game.h"
#include "Camera.h"
#include "IRenderable.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "Renderer.h"
#include "Renderer.tpp"
#include "Mesh.h"

void Renderer::Init()
{
	static DebugVertex vertices[8] = {
		DebugVertex(-0.5f, -0.5f, -0.5f),
		DebugVertex(-0.5f, 0.5f, -0.5f),
		DebugVertex(0.5f, 0.5f, -0.5f),
		DebugVertex(0.5f, -0.5f, -0.5f),
		DebugVertex(-0.5f, -0.5f, 0.5f),
		DebugVertex(-0.5f, 0.5f, 0.5f),
		DebugVertex(0.5f, 0.5f, 0.5f),
		DebugVertex(0.5f, -0.5f, 0.5f),
	};
	static unsigned int indices[12 * 2] = {
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	Mesh<DebugVertex>::MESH_BOX.AddIndices(indices, 12 * 2);
	Mesh<DebugVertex>::MESH_BOX.AddVertices(vertices, 8);
}