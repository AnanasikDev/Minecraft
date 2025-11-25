#include "BlocksDatabase.h"

void BlocksDatabase::Init()
{
	Register(Block::ID::Air, false, [](const BlockData& block, const Context& ctx)
		{
			block.Draw(ctx, TextureAtlas::TextureID::Stone);
		});

	Register(Block::ID::Dirt, true, [](const BlockData& block, const Context& ctx)
		{
			block.Draw(ctx, TextureAtlas::TextureID::Dirt);
		});

	Register(Block::ID::Grass, true, [](const BlockData& block, const Context& ctx)
		{
			switch (ctx.vec)
			{
			case GridVec::Right:
			case GridVec::Left:
			case GridVec::Front:
			case GridVec::Back:
			{
				//block.Draw(ctx, TextureAtlas::TextureID::GrassBlockSide);

				// not really needed, better just generate dirt instead of grass if there is a block on top
				const Block* top = block.At(ctx, GridVec::Top);
				if (top && top->IsSolid())
				{
					block.Draw(ctx, TextureAtlas::TextureID::Dirt);
				}
				else
				{
					block.Draw(ctx, TextureAtlas::TextureID::GrassBlockSide);
				}
			}
				break;
			case GridVec::Top:
				block.Draw(ctx, TextureAtlas::TextureID::GrassBlockTop);
				break;
			case GridVec::Bottom:
				block.Draw(ctx, TextureAtlas::TextureID::Dirt);
				break;
			}
		});

	Register(Block::ID::Stone, true, [](const BlockData& block, const Context& ctx)
		{
			block.Draw(ctx, TextureAtlas::TextureID::Stone);
		});
}

void BlocksDatabase::Register(Block::ID id, bool isSolid, const std::function<void(const BlockData& data, const Context& ctx)>& geomfunc)
{
	m_datas.emplace(id, id);
	BlockData* b = &m_datas[id];
	b->SetGeomFunc(geomfunc);
	b->isSolid = isSolid;
}

BlockData* BlocksDatabase::Get(Block::ID id)
{
	return &m_datas.at(id);
}