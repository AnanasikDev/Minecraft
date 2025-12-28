#pragma once

#include <string>
#include <filesystem>

class AssetManager
{
protected:
	std::filesystem::path m_assetRoot;

public:
	AssetManager() = default;

	virtual ~AssetManager() = default;
	virtual std::filesystem::path GetExecutableDir() = 0;
	std::filesystem::path GetAssetPath(std::string file);
	std::string GetAssetPathString(std::string file);
};