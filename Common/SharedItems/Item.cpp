#include "Item.h"
#include <ImGui-master/imgui.h>
#include <sstream>

void Item::Render(glm::vec2 position, float scale)
{
    if (m_id == ID::None) return;

    assert(m_atlas != nullptr);
    assert(scale > 0.01f);
    
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoInputs;

    glm::vec2 size{ UI_SIZE * scale, UI_SIZE * scale };

    ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    UVRect rect = m_atlas->GetUVs(IntID(m_id));
    ImVec2 uv0 = ImVec2(rect.uv0.x, rect.uv0.y);
    ImVec2 uv1 = ImVec2(rect.uv1.x, rect.uv1.y);
    //ImVec2 uv0 = ImVec2(0, 0);
    //ImVec2 uv1 = ImVec2(1, 1);

    ImVec2 displaySize = ImVec2(size.x, size.y);

    std::stringstream ss;
    ss << "item#";
    ss << position.x;
    ss << position.y;
    ImGui::Begin(ss.str().c_str(), nullptr, window_flags);
    ImGui::Image((ImTextureID)(intptr_t)m_atlas->m_id, displaySize, uv0, uv1);
    ImGui::End();

    ImGui::PopStyleVar(2);
}

std::optional<Block::ID> Item::ItemToBlock(Item::ID itemid)
{
    switch (itemid)
    {
    case Item::ID::DirtBlock: return Block::ID::Dirt;
    case Item::ID::GrassBlock: return Block::ID::Grass;
    case Item::ID::StoneBlock: return Block::ID::Stone;
    case Item::ID::OakLogBlock: return Block::ID::OakLog;
    case Item::ID::OakLeavesBlock: return Block::ID::OakLeaves;
    case Item::ID::SandBlock: return Block::ID::Sand;
    case Item::ID::BedrockBlock: return Block::ID::Bedrock;
    case Item::ID::WaterBlock: return Block::ID::Water;
    case Item::ID::FirLogBlock: return Block::ID::FirLog;
    case Item::ID::FirLeavesBlock: return Block::ID::FirLeaves;
    case Item::ID::LampBlock: return Block::ID::Lamp;
    case Item::ID::TNTBlock: return Block::ID::TNT;
    case Item::ID::None: return std::nullopt;
    default: return std::nullopt;
    }
}

std::optional<Item::ID> Item::BlockToItem(Block::ID itemid)
{
    switch (itemid)
    {
    case Block::ID::Dirt: return Item::ID::DirtBlock;     
    case Block::ID::Grass: return Item::ID::GrassBlock;    
    case Block::ID::Stone: return Item::ID::StoneBlock;    
    case Block::ID::OakLog: return Item::ID::OakLogBlock;   
    case Block::ID::OakLeaves: return Item::ID::OakLeavesBlock;
    case Block::ID::Sand: return Item::ID::SandBlock;     
    case Block::ID::Bedrock: return Item::ID::BedrockBlock;  
    case Block::ID::Water: return Item::ID::WaterBlock;    
    case Block::ID::FirLog: return Item::ID::FirLogBlock;   
    case Block::ID::FirLeaves: return Item::ID::FirLeavesBlock;
    case Block::ID::Lamp: return Item::ID::LampBlock;
    case Block::ID::TNT: return Item::ID::TNTBlock;
    default: return std::nullopt;
    }
}
