#pragma once

#include "Face.h"
#include "Vertex.h"
#include "commons.h"
#include "Mesh.h"
#include "Gamerules.h"

class Renderer;
struct Program;
class AssetManager;

class RendererHelper
{
public:
	RendererHelper() = default;
	~RendererHelper() = default;

	void Init();
	void Update(Gamerules& gamerules);

	void DisableDepth();
	void EnableDepth();
	void ClearScreen();

	void SetupPrograms(const AssetManager* const assets);
	std::unique_ptr<Program> m_spriteProgram;
	std::unique_ptr<Program> m_mainProgram;
	std::unique_ptr<Program> m_waterProgram;

private:

	void CreateProgram(Program* program, std::string vertPath, std::string fragPath);
};

template <typename Vertex>
class VRendererHelper : public RendererHelper
{
};

template <>
class VRendererHelper<FVertex>;

template <>
class VRendererHelper<FVertex> : public RendererHelper
{
public:
	static void AddFace(MeshBase<FVertex>& mesh, glm::ivec3 pos, GridVec vec, TextureAtlas::TextureID texid, unsigned int light)
	{
		Face<FVertex> face = GenerateFace(pos, vec, texid, light, mesh.GetVerticesCount());
		mesh.AddFace(face);
	}

	static Face<FVertex> GenerateFace(FVertex v00, FVertex v10, FVertex v11, FVertex v01, int index)
	{
		std::array<FVertex, 4> v{ v00, v10, v11, v01 };
		std::array<unsigned int, 6> e{ index + 3, index + 2, index, index + 2, index + 1, index };
		return Face<FVertex>{ v, e };
	}

	static Face<FVertex> GenerateFace(glm::ivec3 pos, GridVec vec, TextureAtlas::TextureID texid, unsigned int light, int index)
	{
		switch (vec)
		{
		case GridVec::Back:
		{
			FVertex v00(pos.x,		pos.y,		pos.z,		1, 1, texid, light);
			FVertex v10(pos.x + 1,	pos.y,		pos.z,		0, 1, texid, light);
			FVertex v11(pos.x + 1,	pos.y + 1,	pos.z,		0, 0, texid, light);
			FVertex v01(pos.x,		pos.y + 1,	pos.z,		1, 0, texid, light);
			return GenerateFace(v00, v10, v11, v01, index);
		}
		case GridVec::Front:
		{
			FVertex v00(pos.x,		pos.y,		pos.z + 1,	1, 1, texid, light);
			FVertex v10(pos.x + 1,	pos.y,		pos.z + 1,	0, 1, texid, light);
			FVertex v11(pos.x + 1,	pos.y + 1,	pos.z + 1,	0, 0, texid, light);
			FVertex v01(pos.x,		pos.y + 1,	pos.z + 1,	1, 0, texid, light);
			return GenerateFace(v01, v11, v10, v00, index);
		}
		case GridVec::Right:
		{
			FVertex v00(pos.x + 1,	pos.y + 1,	pos.z,		1, 0, texid, light);
			FVertex v10(pos.x + 1,	pos.y,		pos.z,		1, 1, texid, light);
			FVertex v11(pos.x + 1,	pos.y,		pos.z + 1,	0, 1, texid, light);
			FVertex v01(pos.x + 1,	pos.y + 1,	pos.z + 1,	0, 0, texid, light);
			return GenerateFace(v00, v10, v11, v01, index);
		}
		case GridVec::Left:
		{
			FVertex v00(pos.x,		pos.y + 1,	pos.z,		1, 0, texid, light);
			FVertex v10(pos.x,		pos.y,		pos.z,		1, 1, texid, light);
			FVertex v11(pos.x,		pos.y,		pos.z + 1,	0, 1, texid, light);
			FVertex v01(pos.x,		pos.y + 1,	pos.z + 1,	0, 0, texid, light);
			return GenerateFace(v01, v11, v10, v00, index);
		}
		case GridVec::Top:
		{
			FVertex v00(pos.x,		pos.y + 1,	pos.z,		0, 0, texid, light);
			FVertex v10(pos.x + 1,	pos.y + 1,	pos.z,		1, 0, texid, light);
			FVertex v11(pos.x + 1,	pos.y + 1,	pos.z + 1,	1, 1, texid, light);
			FVertex v01(pos.x,		pos.y + 1,	pos.z + 1,	0, 1, texid, light);
			return GenerateFace(v00, v10, v11, v01, index);
		}
		case GridVec::Bottom:
		{
			FVertex v00(pos.x,		pos.y,		pos.z,		0, 0, texid, light);
			FVertex v10(pos.x + 1,	pos.y,		pos.z,		1, 0, texid, light);
			FVertex v11(pos.x + 1,	pos.y,		pos.z + 1,	1, 1, texid, light);
			FVertex v01(pos.x,		pos.y,		pos.z + 1,	0, 1, texid, light);
			return GenerateFace(v01, v11, v10, v00, index);
		}
		}
	}
};