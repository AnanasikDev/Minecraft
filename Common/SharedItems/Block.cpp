#include "World.h"
#include "Chunk.h"
#include "Collider.h"
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

bool BlockData::IsAir() const
{
	return IsAir(m_id);
}

bool BlockData::IsValid(Block::ID id)
{
	return id != Block::ID::INVALID;
}

bool BlockData::IsValid() const
{
	return IsValid(m_id);
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

bool BlockData::IsWater(Block::ID id)
{
	return id == Block::ID::Water;
}

void BlockData::GenerateGeometry(const GeomContext& ctx)
{
	generateGeometry(*this, ctx);
}

BlockData* Block::GetData() const
{
	return BlocksDatabase::Get(m_id);
}

void Block::Set(Block::ID id, bool updateLight)
{
	m_id = id;
	if (updateLight)
	{
		m_light = BlocksDatabase::Get(id)->m_emission;
	}
}

unsigned char Block::GetEmission() const
{
	return GetData()->m_emission; // could also use block state, like On/Off
}

bool Block::IsLightEmitter() const
{
	return GetEmission() > 0;
}

Collider* Block::GetCollider()
{
	return &(GetData()->m_collider);
}
