#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include "commons.h"
#include "BlockData.h"
#include "Block.h"

class Game;

class BlocksDatabase
{
private:
	static std::vector<BlockData> m_datas;
	static Game* m_game;

public:
	static void Init(Game* game);
	static BlockData* Register(Block::ID id, std::string name, bool isSolid);
	static BlockData* Get(Block::ID id);
};
