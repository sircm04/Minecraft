#pragma once

#include "../../pch.h"

struct BlockFace
{
	struct Vertex
	{
		glm::vec3 position,
			texcoords,
			normal,
			perpendicularNormal1,
			perpendicularNormal2;
	};

	std::array<Vertex, 4> vertices;
};

static inline constexpr BlockFace FRONT_BLOCK_FACE = {
	glm::vec3 { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	glm::vec3 { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f },
	glm::vec3 { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f },
	glm::vec3 { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }
};

static inline constexpr BlockFace BACK_BLOCK_FACE = {
	glm::vec3 { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f },
	glm::vec3 { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	glm::vec3 { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }
};

static inline constexpr BlockFace LEFT_BLOCK_FACE = {
	glm::vec3 { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, -1.0f}, { 0.0f, -1.0f, 0.0f },
	glm::vec3 { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f}, { -1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f}, { 0.0f, -1.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f}, { -1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f}, { -1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, -1.0f}, { 0.0f, 1.0f, 0.0f }
};

static inline constexpr BlockFace RIGHT_BLOCK_FACE = {
	glm::vec3 { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f },
	glm::vec3 { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f },
	glm::vec3 { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f },
	glm::vec3 { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }
};

static inline constexpr BlockFace TOP_BLOCK_FACE = {
	glm::vec3 { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f },
	glm::vec3 { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f },
	glm::vec3 { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }
};

static inline constexpr BlockFace BOTTOM_BLOCK_FACE = {
	glm::vec3 { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f },
	glm::vec3 { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f },
	glm::vec3 { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f },
	glm::vec3 { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }
};

enum class BlockType : uint8_t
{
	Air,
	Grass,
	Count
};

struct Block
{
	BlockType type;
};
