#pragma once

#include "../../pch.h"

struct BlockFace
{
	struct Vertex
	{
		glm::vec3 position,
			normal,
			perpendicularNormal1,
			perpendicularNormal2;

		glm::vec2 texcoords;
	};

	std::array<Vertex, 4> vertices;
};

static inline constexpr BlockFace FRONT_BLOCK_FACE = {
	glm::vec3 { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, glm::vec2 { 0.0f, 1.0f },
	glm::vec3 { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, glm::vec2 { 0.0f, 0.0f },
	glm::vec3 { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, glm::vec2 { 1.0f, 0.0f },
	glm::vec3 { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, glm::vec2 { 1.0f, 1.0f }
};

static inline constexpr BlockFace BACK_BLOCK_FACE = {
	glm::vec3 { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, glm::vec2 { 0.0f, 0.0f },
	glm::vec3 { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, glm::vec2 { 1.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, glm::vec2 { 1.0f, 1.0f },
	glm::vec3 { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, glm::vec2 { 0.0f, 1.0f }
};

static inline constexpr BlockFace LEFT_BLOCK_FACE = {
	glm::vec3 { 0.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, glm::vec2 { 0.0f, 0.0f },
	glm::vec3 { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, glm::vec2 { 1.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, glm::vec2 { 1.0f, 1.0f },
	glm::vec3 { 0.0f, 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, glm::vec2 { 0.0f, 1.0f }
};

static inline constexpr BlockFace RIGHT_BLOCK_FACE = {
	glm::vec3 { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, glm::vec2 { 0.0f, 1.0f },
	glm::vec3 { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, glm::vec2 { 0.0f, 0.0f },
	glm::vec3 { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, glm::vec2 { 1.0f, 0.0f },
	glm::vec3 { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, glm::vec2 { 1.0f, 1.0f }
};

static inline constexpr BlockFace TOP_BLOCK_FACE = {
	glm::vec3 { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, glm::vec2 { 0.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, glm::vec2 { 1.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, glm::vec2 { 1.0f, 1.0f },
	glm::vec3 { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, glm::vec2 { 0.0f, 1.0f }
};

static inline constexpr BlockFace BOTTOM_BLOCK_FACE = {
	glm::vec3 { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, glm::vec2 { 1.0f, 0.0f },
	glm::vec3 { 0.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, glm::vec2 { 1.0f, 1.0f },
	glm::vec3 { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, glm::vec2 { 0.0f, 1.0f },
	glm::vec3 { 1.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, glm::vec2 { 0.0f, 0.0f }
};

enum class BlockType : uint8_t
{
	Air,
	Grass,
	Count
};

struct BlockTypeData
{
	const char* name;
	const std::optional<std::array<int, 6>> faces;
	const bool isSolid = true, isTransparent = false;
};

// faces: front - back - left - right - top - bottom

static constexpr BlockTypeData BLOCK_TYPE_DATA[] =
{
	{ "Air", std::nullopt, false, true },
	{ "Grass", std::array { 0, 0, 0, 0, 1, 2 } }
};

static const BlockTypeData& GetBlockTypeData(BlockType blockType)
{
	return BLOCK_TYPE_DATA[static_cast<uint8_t>(blockType)];
}

struct Block
{
	BlockType type;

	const BlockTypeData& GetBlockTypeData() const
	{
		return BLOCK_TYPE_DATA[static_cast<uint8_t>(type)];
	}
};
