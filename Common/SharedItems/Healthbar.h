#pragma once

#include "Game.h"
#include "UIElement.h"

class Healthbar : public UIElement
{
public:
	std::vector<UIElement> m_UIhearts;
	int m_maxUIhearts{ 10 };
	int m_maxValue{ 20 };
	int m_value{ 15 };

	static constexpr float UI_VERTICAL_SHIFT{ 0.0f };
	static constexpr float HEART_PIXEL_GAP{ 2.0f };

	Healthbar(Game* game) : UIElement(game)
	{
	}
	~Healthbar() override = default;

	void Init() override;
	void Set(int value);
	void Render() override;
};