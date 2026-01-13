#include "AssetManager.h"
#include "Game.h"
#include "TextureAtlas.h"
#include "Hotbar.h"
#include "Healthbar.h"
#include "Console.h"
#include "UI.h"

UI::UI()
{

}

UI::~UI()
{

}

void UI::Init(Game* game)
{
	m_game = game;
	assert(m_game != nullptr);

	m_itemsAtlas = std::make_unique<TextureAtlas>();
	m_itemsAtlas->Create2D(m_game->m_assetManager->GetAssetPathString("Textures/items.png"), GL_REPEAT, 16);
	m_hotbar = std::make_unique<Hotbar>(m_game);
	m_hotbar->Init(m_itemsAtlas.get());
	m_healthbar = std::make_unique<Healthbar>(m_game);
	m_healthbar->Init();
	m_console = std::make_unique<Console>();
	m_console->Init(m_game);
}

void UI::Render()
{
	m_hotbar->Render();
	m_healthbar->Render();
	m_console->Render();
}

void UI::Update()
{
	m_console->Update();
}