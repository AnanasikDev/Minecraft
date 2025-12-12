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
#include "RendererHelper.h"
#include "Renderer.h"

void RendererHelper::Init()
{
	static DebugVertex debugVertices[8] = {
		DebugVertex(-0.5f, -0.5f, -0.5f),
		DebugVertex(-0.5f, 0.5f, -0.5f),
		DebugVertex(0.5f, 0.5f, -0.5f),
		DebugVertex(0.5f, -0.5f, -0.5f),
		DebugVertex(-0.5f, -0.5f, 0.5f),
		DebugVertex(-0.5f, 0.5f, 0.5f),
		DebugVertex(0.5f, 0.5f, 0.5f),
		DebugVertex(0.5f, -0.5f, 0.5f),
	};
	static unsigned int debugIndices[12 * 2] = {
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

	Mesh<DebugVertex>::MESH_BOX.AddIndices(debugIndices, 12 * 2);
	Mesh<DebugVertex>::MESH_BOX.AddVertices(debugVertices, 8);

	VRendererHelper<FVertex>::AddFace<FVertex>(Mesh<FVertex>::MESH_BOX, glm::ivec3(0, 0, 0), GridVec::Front, TextureAtlas::TextureID::Dirt, 15);
	VRendererHelper<FVertex>::AddFace<FVertex>(Mesh<FVertex>::MESH_BOX, glm::ivec3(0, 0, 0), GridVec::Back, TextureAtlas::TextureID::Dirt, 15);
	VRendererHelper<FVertex>::AddFace<FVertex>(Mesh<FVertex>::MESH_BOX, glm::ivec3(0, 0, 0), GridVec::Top, TextureAtlas::TextureID::Dirt, 15);
	VRendererHelper<FVertex>::AddFace<FVertex>(Mesh<FVertex>::MESH_BOX, glm::ivec3(0, 0, 0), GridVec::Bottom, TextureAtlas::TextureID::Dirt, 15);
	VRendererHelper<FVertex>::AddFace<FVertex>(Mesh<FVertex>::MESH_BOX, glm::ivec3(0, 0, 0), GridVec::Right, TextureAtlas::TextureID::Dirt, 15);
	VRendererHelper<FVertex>::AddFace<FVertex>(Mesh<FVertex>::MESH_BOX, glm::ivec3(0, 0, 0), GridVec::Left, TextureAtlas::TextureID::Dirt, 15);
}
