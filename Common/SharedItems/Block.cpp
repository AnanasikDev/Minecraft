#include "Chunk.h"
#include "Block.h"

const Block* BlockData::At(const Context& ctx, GridVec vec) const
{
	glm::ivec3 v = GridVecToVec3(vec);
	return ctx.chunk->At(ctx.x + v.x, ctx.y + v.y, ctx.z + v.z);
}

Block::ID BlockData::IDAt(const Context& ctx, GridVec vec) const
{
	glm::ivec3 v = GridVecToVec3(vec);
	const Block* block = ctx.chunk->At(ctx.x + v.x, ctx.y + v.y, ctx.z + v.z);
	if (block) return block->m_data->id;
	return Block::ID::INVALID;
}

void BlockData::Draw(const Context& ctx, TextureAtlas::TextureID textureid) const
{
	const Block* b = At(ctx, ctx.vec);
	if (b && b->IsSolid()) return;

	switch (ctx.vec)
	{
	case GridVec::Right:
		ctx.chunk->AddRight(ctx.x, ctx.y, ctx.z, textureid);
		break;
	case GridVec::Left:
		ctx.chunk->AddLeft(ctx.x, ctx.y, ctx.z, textureid);
		break;
	case GridVec::Top:
		ctx.chunk->AddTop(ctx.x, ctx.y, ctx.z, textureid);
		break;
	case GridVec::Bottom:
		ctx.chunk->AddBottom(ctx.x, ctx.y, ctx.z, textureid);
		break;
	case GridVec::Front:
		ctx.chunk->AddFront(ctx.x, ctx.y, ctx.z, textureid);
		break;
	case GridVec::Back:
		ctx.chunk->AddBack(ctx.x, ctx.y, ctx.z, textureid);
		break;
	default:
		break;
	}
}

void BlockData::GenerateGeometry(const Context& ctx)
{
	generateGeometry(*this, ctx);
}

Block::Block(BlockData* data) : m_data(data)
{

}

bool Block::IsSolid() const
{
	return m_data->isSolid;
}

bool Block::IsAir() const
{
	return m_data->id == ID::Air;
}

bool Block::IsValid() const
{
	return m_data->id != ID::INVALID;
}

void Block::Set(BlockData* data)
{
	m_data = data;
}