#include "AssetManager.h"

std::filesystem::path AssetManager::GetAssetPath(std::string file)
{
	return m_assetRoot / file;
}

std::string AssetManager::GetAssetPathString(std::string file)
{
	return (m_assetRoot / file).string();
}