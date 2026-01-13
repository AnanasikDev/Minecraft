#include "Healthbar.h"
#include "AssetManager.h"

void Healthbar::Init()
{
	assert(m_game != nullptr);
	
	m_scale = 4.0f;
	float heartPixelWidth = 8.0f + HEART_PIXEL_GAP;
	float totalWidth = static_cast<float>(m_maxUIhearts) * (heartPixelWidth * m_scale);

	float startX = (Game::WINDOW_WIDTH / 2.0f) - (totalWidth / 2.0f);
	float startY = Game::WINDOW_HEIGHT - (8 * m_scale) - (UI_VERTICAL_SHIFT + 200.0f);
	for (int i = 0; i < m_maxUIhearts; i++)
	{
		m_UIhearts.emplace_back(m_game);
		m_UIhearts[i].LoadSprite(m_game->m_assetManager->GetAssetPathString("Textures/hearts.png"));
		m_UIhearts[i].m_scale = m_scale;
		m_UIhearts[i].m_position = 
			glm::vec2(startX, startY) + 
			glm::vec2(heartPixelWidth * m_scale * static_cast<float>(i), 0.0f);
		m_UIhearts[i].m_size = glm::vec2(8, 8);
		m_UIhearts[i].AutoName();
		m_UIhearts[i].Init();
	}
	
	Set(m_value);
}

void Healthbar::Set(int value)
{
	m_value = value;
	if (m_value < 0) m_value = 0;
	if (m_value > m_maxValue) m_value = m_maxValue;
}

void Healthbar::Render()
{
	int heartsLeft{ m_value };
	for (int i = 0; i < m_maxUIhearts; i++)
	{
		UVRect uvs;
		if (heartsLeft > 1)
		{
			uvs = m_UIhearts[i].GetTextureRef().GetUVs(0, 8);
			heartsLeft -= 2;
		}
		else if (heartsLeft == 1)
		{
			uvs = m_UIhearts[i].GetTextureRef().GetUVs(1, 8);
			heartsLeft -= 1;
		}
		else
		{
			uvs = m_UIhearts[i].GetTextureRef().GetUVs(2, 8);
		}

		m_UIhearts[i].Render(uvs.uv0, uvs.uv1);
	}
}