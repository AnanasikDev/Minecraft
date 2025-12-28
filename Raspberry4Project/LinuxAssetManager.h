#pragma once

#include "AssetManager.h"

class LinuxAssetManager : public AssetManager
{
public:
	LinuxAssetManager();
	std::filesystem::path GetExecutableDir() override;
};
