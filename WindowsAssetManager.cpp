#include "AssetManager.h"
#include <Windows.h>
#include "WindowsAssetManager.h"

WindowsAssetManager::WindowsAssetManager() : AssetManager()
{
    m_assetRoot = GetExecutableDir() / "Assets";
}

std::filesystem::path WindowsAssetManager::GetExecutableDir()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
}
