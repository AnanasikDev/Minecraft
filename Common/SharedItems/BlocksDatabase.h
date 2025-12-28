#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include "commons.h"
#include "BlockData.h"
#include "Block.h"

class BlocksDatabase
{
private:
	static std::vector<BlockData> m_datas;
	//static std::unordered_map<Block::ID, BlockData> m_datas;
public:

	static void Init();
	static BlockData* Register(Block::ID id, std::string name, bool isSolid);
	static BlockData* Get(Block::ID id);
};
