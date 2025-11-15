#pragma once

#include <unordered_map>
#include "BlockData.h"
#include "Block.h"

class Blocks
{
public:
	std::unordered_map<Block::ID, BlockData> m_datas;

	void Init();
	void Register(Block::ID id, const std::function<void(const BlockData& data, const Context& ctx)>& geomfunc);
	BlockData* Get(Block::ID id);
};
