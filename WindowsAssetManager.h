#pragma once

#include "AssetManager.h"

class WindowsAssetManager : public AssetManager
{
public:
	WindowsAssetManager();
	std::filesystem::path GetExecutableDir() override;
};
