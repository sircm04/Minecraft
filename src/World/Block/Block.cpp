#include "../../pch.h"
#include "Block.h"

const BlockTypeData& Block::GetBlockTypeData() const noexcept
{
	return BLOCK_TYPE_DATA[static_cast<uint8_t>(m_BlockType)];
}