#include "commons.h"
#include "Game.h"
#include "Player.h"
#include "Console.h"
#include "World.h"
#include "Input.h"
#include "IInput.h"
#include "IGraphics.h"

void Console::Init(Game* game)
{
    m_game = game;
    m_backend.RegisterCommand("tp", "tp <x> <y> <z>", std::function<void(float, float, float)>(
        [&](float x, float y, float z)
        {
            m_game->m_player->m_transform.SetLocalPosition({ x, y, z });
        }
    ));

    m_backend.RegisterCommand("translate", "translate <x> <y> <z>", std::function<void(float, float, float)>(
        [&](float x, float y, float z)
        {
            m_game->m_player->m_transform.Translate({ x, y, z });
        }
    ));

    m_backend.RegisterCommand("quit", "Exits game", std::function<void()>(
        [&]() { m_game->Quit(); }
    ));

    m_backend.RegisterCommand("time_set", "time_set <seconds>", std::function<void(int)>(
        [&](int t)
        {
            GameTime::SetDayTime(static_cast<float>(t) / static_cast<float>(GameTime::DAY_SECONDS));
        }
    ));

    m_backend.RegisterCommand("day_duration", "time_set <seconds>", std::function<void(float)>(
        [&](float t)
        {
            GameTime::DAY_SECONDS = t;
        }
    ));

    m_backend.RegisterCommand("set_gravity", "set_gravity <value>", std::function<void(float)>(
        [&](float value)
        {
            m_game->GetGamerules().m_gravity = value;
        }
    ));

    m_backend.RegisterCommand("set_block", "set_block <x> <y> <z> <id>", std::function<void(int, int, int, std::string)>(
        [&](int x, int y, int z, std::string id)
        {
            Block::ID blockid{ Block::NameToID(id) };
            if (blockid == Block::ID::INVALID) return;

            m_game->m_world->SetAndUpdateBlockAtWorld(glm::ivec3(x, y, z), blockid);
        }
    ));

    m_backend.RegisterCommand("set_block_at_player", "set_block_at_playe <id>", std::function<void(std::string)>(
        [&](std::string id)
        {
            Block::ID blockid{ Block::NameToID(id) };
            if (blockid == Block::ID::INVALID) return;

            m_game->m_world->SetAndUpdateBlockAtWorld(World::SnapToBlock(m_game->m_player->m_transform.GetLocalPosition()), blockid);
        }
    ));

    m_backend.RegisterCommand("help", "Displays all available commands", std::function<void()>(
        [&]()
        {
            m_backend.AddLog("--- Available Commands ---");
            for (auto const& [name, cmd] : m_backend.m_commands)
            {
                m_backend.AddLog(name + " : " + cmd.m_name);
            }
        }
    ));
}

void Console::Render()
{
    if (!m_isOpen) return;

    if (!ImGui::Begin("Console", &m_isOpen))
    { 
        ImGui::End();
        return;
    }

    float reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -reserve), false);

    for (const auto& log : m_backend.m_history)
    {
        ImGui::TextUnformatted(log.c_str());
    }

    static char tempBuf[256] = "";
    std::string currentInput(tempBuf);

    if (!currentInput.empty())
    {
        ImGui::Separator();
        ImGui::TextDisabled("Suggestions:");

        for (auto const& [name, cmd] : m_backend.m_commands)
        {
            if (name.compare(0, currentInput.length(), currentInput) == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
                ImGui::Text("> %s", name.c_str());
                ImGui::PopStyleColor();

                ImGui::SameLine();
                ImGui::TextDisabled("- %s", cmd.m_name.c_str());
            }
        }
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::Separator();

    if (ImGui::InputText("##Input", tempBuf, IM_ARRAYSIZE(tempBuf), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_backend.Execute(tempBuf);
        tempBuf[0] = '\0';
        ImGui::SetKeyboardFocusHere(-1);
    }

    if (ImGui::IsWindowAppearing())
    {
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::End();
}

void Console::Update()
{
    
    if (m_game->GetInput().GetKeyboard().IsKeyPressed(Key::X))
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) return;
        Toggle(!m_isOpen);
    }
    else if (m_isOpen && m_game->GetInput().GetKeyboard().IsKeyPressed(Key::ESCAPE))
    {
        Toggle(false);
    }
}

void Console::Toggle(bool state)
{
    m_isOpen = state;
    if (!m_isOpen)
    {
        ImGui::SetWindowFocus(nullptr);
        m_game->GetGraphics()->LockCursor();
    }
}
