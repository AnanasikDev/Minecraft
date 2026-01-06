#include <unistd.h>
#include <filesystem>
#include "LinuxAssetManager.h"

LinuxAssetManager::LinuxAssetManager() : AssetManager()
{
    m_assetRoot = GetExecutableDir() / "../Common/Assets";
}

std::filesystem::path LinuxAssetManager::GetExecutableDir() const
{
    char buffer[4096];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1)
        return {};

    buffer[len] = '\0';
    return std::filesystem::path(buffer).parent_path();
}
