#pragma once

#include <array>
#include "UIElement.h"
#include "Item.h"

class Player;
class TextureAtlas;

class Hotbar : public UIElement
{
public:
	static constexpr int SLOTS_NUM{ 6 };
	static constexpr float UI_VERTICAL_SHIFT{ 90 };
	static constexpr int RESOLUTION{ 8 };

	UIElement m_selector;
	std::array<Item, SLOTS_NUM> m_slots;
	TextureAtlas* m_itemAtlas{ nullptr };

	Hotbar(Game* game) : UIElement(game), m_selector(game)
	{
	}
	~Hotbar() override = default;

	using UIElement::Init;
	void Init(TextureAtlas* atlas);
	void Render() override;
	Item Select(int i, Player* const player);
	Item Reselect(Player* const player);
	Item& Selection();
	void Set(int i, Item::ID item);
	void SetCurrent(Item::ID item);
	int Scroll(int power);
};