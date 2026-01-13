#include "Player.h"
#include "Game.h"
#include "AssetManager.h"
#include "Hotbar.h"

void Hotbar::Init(TextureAtlas* atlas)
{
	assert(m_game != nullptr);
	LoadSprite(m_game->m_assetManager->GetAssetPathString("Textures/hotbar.png"));
	m_size = glm::vec2(RESOLUTION * SLOTS_NUM, RESOLUTION);
	m_scale = 10;
	m_position = glm::vec2(Game::WINDOW_WIDTH / 2.0f - 0.5f * m_size.x * m_scale, Game::WINDOW_HEIGHT - m_size.y * m_scale - UI_VERTICAL_SHIFT);
	m_name = "hotbar";

	m_selector.Init();
	m_selector.LoadSprite(m_game->m_assetManager->GetAssetPathString("Textures/hotbar_selector.png"));
	m_selector.m_position = m_position - glm::vec2(1, 1) * m_scale;
	m_selector.m_size = glm::vec2(10, 10);
	m_selector.m_scale = m_scale;
	m_selector.m_name = "hotbar_selector";

	m_itemAtlas = atlas;
	m_slots[0] = Item(Item::ID::DirtBlock,	m_itemAtlas);
	m_slots[1] = Item(Item::ID::GrassBlock,	m_itemAtlas);
	m_slots[2] = Item(Item::ID::LampBlock,	m_itemAtlas);
	m_slots[3] = Item(Item::ID::StoneBlock,	m_itemAtlas);
	m_slots[4] = Item(Item::ID::None,		m_itemAtlas);
	m_slots[5] = Item(Item::ID::TNTBlock,	m_itemAtlas);
}

void Hotbar::Render()
{
	UIElement::Render();
	m_selector.Render();

	glm::vec2 shift = glm::vec2((glm::vec2(GetFinalSize().y) - glm::vec2(Item::UI_SIZE)) / 2.0f);

	for (int i = 0; i < SLOTS_NUM; i++)
	{
		m_slots[i].Render(m_position + glm::vec2(RESOLUTION, 0) * m_scale * static_cast<float>(i) + shift, 1);
	}
}

Item Hotbar::Select(int i, Player* const player)
{
	if (i < 0 || i > SLOTS_NUM) return Item();

	const int deltaIndex{ i - m_index };

	m_selector.m_position += glm::vec2(RESOLUTION * m_scale, 0) * static_cast<float>(deltaIndex);
	m_index = i;

	std::optional<Block::ID> blockid = Item::ItemToBlock(Selection().m_id);
	if (blockid.has_value())
	{
		player->SelectBlock(blockid.value());
	}

	return m_slots[i];
}

Item Hotbar::Reselect(Player* const player)
{
	return Select(m_index, player);
}

Item& Hotbar::Selection()
{
	return m_slots[m_index];
}

void Hotbar::Set(int i, Item::ID item)
{
	m_slots[i] = Item(item, m_itemAtlas);
}

void Hotbar::SetCurrent(Item::ID item)
{
	Set(m_index, item);
}

int Hotbar::Scroll(int power)
{
	return AMath::Repeat(m_index + power, SLOTS_NUM);
}
