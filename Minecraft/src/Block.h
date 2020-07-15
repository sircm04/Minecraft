#pragma once

#include <optional>

enum class BlockType : uint8_t
{
	Count = 8,

	Air = 0,
	Grass = 1,
	Dirt = 2,
	Stone = 3,
	Cobblestone = 4,
	Bedrock = 5,
	Wood = 6,
	Log = 7
};

struct BlockTypeData
{
	std::optional<std::vector<float>> m_Faces;
	bool isSolid = true, isTransparent = false;
};

// faces: front - back - left - right - top - bottom

static const std::vector<BlockTypeData> BLOCK_TYPE_DATA =
{
	{ std::nullopt, false, true }, // Air
	{ std::vector<float> { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f } }, // Grass
	{ std::vector<float> { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f } }, // Dirt
	{ std::vector<float> { 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f } }, // Stone
	{ std::vector<float> { 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f } }, // Cobblestone
	{ std::vector<float> { 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f } }, // Bedrock
	{ std::vector<float> { 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f } }, // Wood
	{ std::vector<float> { 7.0f, 7.0f, 7.0f, 7.0f, 8.0f, 8.0f } }, // Log
};

static const BlockTypeData* GetBlockTypeData(BlockType blockType)
{
	return &BLOCK_TYPE_DATA[(unsigned int) blockType];
}

struct Block
{
	BlockType m_BlockType;

	const BlockTypeData* GetBlockTypeData() const
	{
		return &BLOCK_TYPE_DATA[(unsigned int) m_BlockType];
	}
};