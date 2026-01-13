#include "UIElement.h"
#include "ImGui-master/imgui.h"
#include <sstream>

void UIElement::LoadSprite(std::string filename)
{
	m_texture.Create(filename);
}

void UIElement::Render()
{
	Render(glm::vec2(0, 0), glm::vec2(1, 1));
}

void UIElement::Render(glm::vec2 uv0, glm::vec2 uv1)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoInputs;

	glm::vec2 size{ m_size.x * m_scale, m_size.y * m_scale };

	ImGui::SetNextWindowPos(ImVec2(m_position.x, m_position.y));
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin(m_name.c_str(), nullptr, window_flags);
	ImGui::Image((ImTextureID)(intptr_t)m_texture.m_id, ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y));
	ImGui::End();

	ImGui::PopStyleVar(2);
}

Texture& UIElement::GetTextureRef()
{
	return m_texture;
}

void UIElement::AutoName()
{
	std::stringstream ss;
	ss << m_position.x;
	ss << m_position.y;
	ss << m_texture.m_id;
	m_name = ss.str();
}
