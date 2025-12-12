#pragma once

#include <glm/vec3.hpp>
#include "commons.h"
#include "Vertex.h"
#include "Mesh.h"

class Chunk;
struct RemeshRequest;
template <typename Vertex>
struct BaseCompositeMesh;
class World;

struct GeomContext
{
	glm::ivec3 localPos;
	glm::ivec3 globalPos;
	GridVec vec;
	World* const world;
	RemeshRequest* const request;
	BaseCompositeMesh<FVertex>* mesh;

	GeomContext(const glm::ivec3& localPos, const glm::ivec3& globalPos, const GridVec& vec, World* world, RemeshRequest* request, BaseCompositeMesh<FVertex>* mesh)
		: localPos(localPos), globalPos(globalPos), vec(vec), world(world), request(request), mesh(mesh)
	{
	}
};