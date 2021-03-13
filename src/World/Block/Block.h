#pragma once

enum class BlockType : uint8_t
{
	Air,
	Grass,
	Dirt,
	Stone,
	Cobblestone,
	Bedrock,
	Wood,
	Log,

	Count
};

struct BlockTypeData
{
	const std::optional<std::array<float, 6>> m_Faces;
	const bool isSolid = true, isTransparent = false;
};

// faces: front - back - left - right - top - bottom

static constexpr BlockTypeData BLOCK_TYPE_DATA[] =
{
	{ std::nullopt, false, true }, // Air
	{ std::array { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f } }, // Grass
	{ std::array { 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f } }, // Dirt
	{ std::array { 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f } }, // Stone
	{ std::array { 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f } }, // Cobblestone
	{ std::array { 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f } }, // Bedrock
	{ std::array { 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f } }, // Wood
	{ std::array { 7.0f, 7.0f, 7.0f, 7.0f, 8.0f, 8.0f } }, // Log
};

static const BlockTypeData& GetBlockTypeData(BlockType blockType) noexcept
{
	return BLOCK_TYPE_DATA[static_cast<uint8_t>(blockType)];
}

struct Block
{
	BlockType m_BlockType;

	const BlockTypeData& GetBlockTypeData() const noexcept;
};