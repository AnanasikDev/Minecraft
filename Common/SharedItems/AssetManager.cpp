#include "AssetManager.h"

std::filesystem::path AssetManager::GetAssetPath(std::string file) const
{
	return m_assetRoot / file;
}

std::string AssetManager::GetAssetPathString(std::string file) const
{
	return (m_assetRoot / file).string();
}