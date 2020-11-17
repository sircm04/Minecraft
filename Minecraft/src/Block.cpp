#include "pch.h"
#include "Block.h"

static const BlockTypeData& GetBlockTypeData(BlockType blockType)
{
	return BLOCK_TYPE_DATA[static_cast<uint8_t>(blockType)];
}

const BlockTypeData& Block::GetBlockTypeData() const
{
	return BLOCK_TYPE_DATA[static_cast<uint8_t>(m_BlockType)];
}