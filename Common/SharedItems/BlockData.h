#pragma once

#include <functional>
#include "Block.h"
#include "GeomContext.h"
#include "TextureAtlas.h"
#include "Collider.h"

struct BlockData
{
	using funInteraction = std::function<Interaction::Result(Interaction)>;

	// =========

	Block::ID m_id;
	bool m_isSolid : 1;
	bool m_isTransparent : 1;
	bool m_canFall : 1;
	unsigned char m_emission : 4;
	std::string m_name;
	Collider m_collider;

	std::function<void(const BlockData& data, const GeomContext& ctx)> generateGeometry;
	std::optional<funInteraction> m_funInteraction = std::nullopt;
	
	BlockData() : m_id(Block::ID::Air), m_isSolid(true), m_isTransparent(false), m_canFall(false), m_emission(0)
	{
	}
	BlockData(Block::ID id) : m_id(id), m_isSolid(true), m_isTransparent(false), m_canFall(false), m_emission(0)
	{
	}

	void SetGeomFunc(const std::function<void(const BlockData& data, const GeomContext& ctx)>& generateGeometry)
	{
		this->generateGeometry = generateGeometry;
	}

	void GenerateGeometry(const GeomContext& ctx);

	void DrawGridFace(const GeomContext& ctx, TextureAtlas::TextureID textureid) const;

	static bool IsAir(Block::ID id);
	bool IsAir() const;
	static bool IsValid(Block::ID id);
	bool IsValid() const;
	static bool IsSolid(Block::ID id);
	inline bool IsSolid() const
	{
		return m_isSolid;
	}
	static bool IsTransparent(Block::ID id);
	inline bool IsTransparent() const
	{
		return m_isTransparent;
	}
	static bool IsLightSource(Block::ID id);
	inline bool IsLightSource() const
	{
		return m_emission > 0;
	}
	inline bool IsLightable() const
	{
		return IsAir() || IsTransparent() || !IsSolid();
	}
	inline bool IsWater() const
	{
		return IsWater(m_id);
	}
	static bool IsWater(Block::ID id);
	inline bool IsInteractable() const
	{
		return m_funInteraction != std::nullopt;
	}
	static bool IsInteractable(Block::ID id);

	BlockData* AffectedByGravity()
	{
		m_canFall = true;
		return this;
	}
	BlockData* LightSource(unsigned char power)
	{
		m_emission = power & 0xF;
		return this;
	}
	BlockData* Transparent()
	{
		m_isTransparent = true;
		return this;
	}
	BlockData* SimpleTexture(TextureAtlas::TextureID textureid)
	{
		SetGeomFunc([textureid](const BlockData& block, const GeomContext& ctx)
			{
				block.DrawGridFace(ctx, textureid);
			});
		return this;
	}
	BlockData* NoGraphics()
	{
		SetGeomFunc([](const BlockData& block, const GeomContext& ctx)
			{
			});
		return this;
	}
	BlockData* ContextualTexture(const std::function<void(const BlockData& data, const GeomContext& ctx)>& generateGeometry)
	{
		SetGeomFunc(generateGeometry);
		return this;
	}
	BlockData* SetCollider(Collider collider)
	{
		m_collider = collider;
		return this;
	}

	BlockData* Interactable(funInteraction interaction);
};
