#include "World.h"
#include "Chunk.h"
#include "Block.h"
#include "BlocksDatabase.h"
#include "CompositeMesh.h"

void BlockData::DrawGridFace(const GeomContext& ctx, TextureAtlas::TextureID textureid) const
{
	ctx.mesh->AddBlockFace(ctx, textureid);
}

bool BlockData::IsAir(Block::ID id)
{
	return id == Block::ID::Air;
}
bool BlockData::IsValid(Block::ID id)
{
	return id != Block::ID::INVALID;
}
bool BlockData::IsSolid(Block::ID id)
{
	return BlocksDatabase::Get(id)->IsSolid();
}

bool BlockData::IsTransparent(Block::ID id)
{
	return BlocksDatabase::Get(id)->IsTransparent();
}

bool BlockData::IsLightSource(Block::ID id)
{
	return BlocksDatabase::Get(id)->IsLightSource();
}

void BlockData::GenerateGeometry(const GeomContext& ctx)
{
	generateGeometry(*this, ctx);
}

BlockData* Block::GetData() const
{
	return BlocksDatabase::Get(m_id);
}

void Block::Set(Block::ID id)
{
	m_id = id;
	m_light = BlocksDatabase::Get(id)->emission;
}