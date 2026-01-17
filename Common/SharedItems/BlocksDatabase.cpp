#include "BlocksDatabase.h"
#include "Chunk.h"
#include "World.h"
#include "RemeshRequest.h"
#include "Player.h"

std::vector<BlockData> BlocksDatabase::m_datas;
Game* BlocksDatabase::m_game = nullptr;

void BlocksDatabase::Init(Game* game)
{
	m_game = game;
	Register(Block::ID::Air, "Air", false)
		->NoGraphics()
		->Transparent();

	Register(Block::ID::Dirt, "Dirt", true)
		->SimpleTexture(TextureAtlas::TextureID::Dirt)
		->AffectedByGravity()
		->SetCollider(Collider::FullBlock());

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
				if (ctx.request && ctx.request->chunk)
				{
					const Block* top = ctx.request->GetBlockAtWorld(ctx.globalPos + glm::ivec3(0, 1, 0));
					if (top && BlockData::IsSolid(top->GetID()) && !BlockData::IsTransparent(top->GetID()))
					{
						block.DrawGridFace(ctx, TextureAtlas::TextureID::Dirt);
					}
					else
					{
						block.DrawGridFace(ctx, TextureAtlas::TextureID::GrassBlockSide);
					}
				}
				else
				{
					block.DrawGridFace(ctx, TextureAtlas::TextureID::GrassBlockSide);
				}
			}
				break;
			case GridVec::Top:
				block.DrawGridFace(ctx, TextureAtlas::TextureID::GrassBlockTop);
				break;
			case GridVec::Bottom:
				block.DrawGridFace(ctx, TextureAtlas::TextureID::Dirt);
				break;
			default:
				break;
			}
		})
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::Stone, "Stone", true)
		->SimpleTexture(TextureAtlas::TextureID::Stone)
		->AffectedByGravity()
		->SetCollider(Collider::FullBlock());

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
				default:
					break;
				}
			})
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::OakLeaves, "Oak leaves", true)
		->SimpleTexture(TextureAtlas::TextureID::OakLeaves)
		->Transparent()
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::Sand, "Sand", true)
		->SimpleTexture(TextureAtlas::TextureID::Sand)
		->AffectedByGravity()
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::Bedrock, "Bedrock", true)
		->SimpleTexture(TextureAtlas::TextureID::Bedrock)
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::Water, "Water", false)
		->SimpleTexture(TextureAtlas::TextureID::Water)
		->Transparent()
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::FirLog, "Fir log", true)
		->ContextualTexture([](const BlockData& block, const GeomContext& ctx)
		{
			switch (ctx.vec)
			{
			case GridVec::Right:
			case GridVec::Left:
			case GridVec::Front:
			case GridVec::Back:
				block.DrawGridFace(ctx, TextureAtlas::TextureID::FirLogSide);
				break;
			case GridVec::Bottom:
			case GridVec::Top:
				block.DrawGridFace(ctx, TextureAtlas::TextureID::FirLogTop);
				break;
			default:
				break;
			}
		})
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::FirLeaves, "Fir leaves", true)
		->SimpleTexture(TextureAtlas::TextureID::FirLeaves)
		->Transparent()
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::Lamp, "Lamp", true)
		->SimpleTexture(TextureAtlas::TextureID::Lamp)
		->LightSource(15)
		->SetCollider(Collider::FullBlock());

	Register(Block::ID::TNT, "TNT", true)
		->ContextualTexture([](const BlockData& block, const GeomContext& ctx)
			{
				switch (ctx.vec)
				{
				case GridVec::Right:
				case GridVec::Left:
				case GridVec::Front:
				case GridVec::Back:
					block.DrawGridFace(ctx, TextureAtlas::TextureID::TNTSide);
					break;
				case GridVec::Bottom:
					block.DrawGridFace(ctx, TextureAtlas::TextureID::TNTBottom);
					break;
				case GridVec::Top:
					block.DrawGridFace(ctx, TextureAtlas::TextureID::TNTTop);
					break;
				default:
					break;
				}
			})
		->SetCollider(Collider::FullBlock())
		->Interactable([&](Interaction args)
			{
				std::set<Chunk*> chunksToUpdate;
				const float radius{ m_game->GetGamerules().m_TNTExplosionRadius };
				const int iradius{ static_cast<int>(ceil(radius)) };

				for (int x = -iradius; x <= iradius; x++) { for (int y = -iradius; y <= iradius; y++) { for (int z = -iradius; z <= iradius; z++) {
					const glm::ivec3 blockOffset(x, y, z);
					if (glm::length(glm::vec3(blockOffset)) > radius) continue;

					const glm::ivec3 targetPos{ args.raycast.m_worldBlockPos + blockOffset };
					const glm::ivec3 chunkGridPos{ World::WorldBlockToChunkGrid(targetPos) };
					Chunk* const chunk{ args.world.GetChunkAt(chunkGridPos) };

					if (!chunk) continue;

					const glm::ivec3 localPos{ chunk->WorldToLocal(targetPos) };
					if (chunk->AtSafe(localPos)->GetID() == Block::ID::Air) continue;

					chunk->NewBlock(localPos, Block::ID::Air, true);
					chunksToUpdate.insert(chunk);

					if (World::IsLocalBlockOnChunkEdge(localPos))
					{
						std::vector<glm::ivec3> neighborDiffs = chunk->GetChunkNeighboursAt(localPos);
						for (auto& diff : neighborDiffs)
						{
							Chunk* const neighbor = args.world.GetChunkAt(chunk->m_position + diff);
							if (neighbor) chunksToUpdate.insert(neighbor);
						}
					}
				}}}

				for (Chunk* c : chunksToUpdate)
				{
					c->m_isDirty = true;
				}

				const float maxDmg{ m_game->m_player->GetHealth().GetMax() };

				auto _GetDamage = [maxDmg](float r, float d)
				{
					constexpr float p{ 2.0f };
					const float m{ 1.0f - std::powf(d / r, p) };
					return maxDmg * m;
				};

				const float distance{ glm::distance(args.player->m_transform.GetLocalPosition(), args.raycast.m_worldPos) };
				const float damage{ -_GetDamage(radius, distance) };
				args.player->GetHealth().Change(damage);
				return Interaction::Result::Success_Stop;
			});
}

BlockData* BlocksDatabase::Register(Block::ID id, std::string name, bool isSolid)
{
	m_datas.emplace_back(id);
	BlockData* b = Get(id);
	b->m_isSolid = isSolid;
	b->m_name = name;
	return b;
}

BlockData* BlocksDatabase::Get(Block::ID id)
{
	if (id == Block::ID::INVALID) return nullptr;
	int index{ static_cast<int>(id) };
	assert(index < m_datas.size());
	return &m_datas.at(index);
}
