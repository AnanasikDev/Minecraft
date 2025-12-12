#include "BlocksDatabase.h"

std::unordered_map<Block::ID, BlockData> BlocksDatabase::m_datas;


void BlocksDatabase::Init()
{
	Register(Block::ID::Air, "Air", false)
		->NoGraphics()
		->Transparent();

	Register(Block::ID::Dirt, "Dirt", true)
		->SimpleTexture(TextureAtlas::TextureID::Dirt)
		->AffectedByGravity();

	Register(Block::ID::Grass, "Grass", true)
		->ContextualTexture([](const BlockData& block, const GeomContext& ctx)
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
				block.DrawGridFace(ctx, TextureAtlas::TextureID::GrassBlockSide);
				/*const Block* top = block.At(ctx, GridVec::Top);
				if (top && BlockData::IsSolid(top->GetID()))
				{
					block.DrawGridFace(ctx, TextureAtlas::TextureID::Dirt);
				}
				else
				{
					block.DrawGridFace(ctx, TextureAtlas::TextureID::GrassBlockSide);
				}*/
			}
				break;
			case GridVec::Top:
				block.DrawGridFace(ctx, TextureAtlas::TextureID::GrassBlockTop);
				break;
			case GridVec::Bottom:
				block.DrawGridFace(ctx, TextureAtlas::TextureID::Dirt);
				break;
			}
		});

	Register(Block::ID::Stone, "Stone", true)
		->SimpleTexture(TextureAtlas::TextureID::Stone)
		->AffectedByGravity();

	Register(Block::ID::OakLog, "Oak log", true)
		->ContextualTexture([](const BlockData& block, const GeomContext& ctx)
			{
				switch (ctx.vec)
				{
				case GridVec::Right:
				case GridVec::Left:
				case GridVec::Front:
				case GridVec::Back:
				{
					block.DrawGridFace(ctx, TextureAtlas::TextureID::OakLogSide);
				}
				break;
				case GridVec::Bottom:
				case GridVec::Top:
					block.DrawGridFace(ctx, TextureAtlas::TextureID::OakLogTop);
					break;
				}
			});

	Register(Block::ID::OakLeaves, "Oak leaves", true)
		->SimpleTexture(TextureAtlas::TextureID::OakLeaves)
		->Transparent();

	Register(Block::ID::Sand, "Sand", true)
		->SimpleTexture(TextureAtlas::TextureID::Sand)
		->AffectedByGravity();

	Register(Block::ID::Bedrock, "Bedrock", true)
		->SimpleTexture(TextureAtlas::TextureID::Bedrock);

	Register(Block::ID::Water, "Water", false)
		->SimpleTexture(TextureAtlas::TextureID::Water)
		->Transparent();

	Register(Block::ID::FirLog, "Fir log", true)
		->ContextualTexture([](const BlockData& block, const GeomContext& ctx)
		{
			switch (ctx.vec)
			{
			case GridVec::Right:
			case GridVec::Left:
			case GridVec::Front:
			case GridVec::Back:
			{
				block.DrawGridFace(ctx, TextureAtlas::TextureID::FirLogSide);
			}
			break;
			case GridVec::Bottom:
			case GridVec::Top:
				block.DrawGridFace(ctx, TextureAtlas::TextureID::FirLogTop);
				break;
			}
		});

	Register(Block::ID::FirLeaves, "Fir leaves", true)
		->SimpleTexture(TextureAtlas::TextureID::FirLeaves)
		->Transparent();

	Register(Block::ID::Lamp, "Lamp", true)
		->SimpleTexture(TextureAtlas::TextureID::Lamp)
		->LightSource(15);
}

BlockData* BlocksDatabase::Register(Block::ID id, std::string name, bool isSolid)
{
	m_datas.emplace(id, id);
	BlockData* b = &m_datas[id];
	b->isSolid = isSolid;
	b->name = name;
	return b;
}

BlockData* BlocksDatabase::Get(Block::ID id)
{
	return &m_datas.at(id);
}
