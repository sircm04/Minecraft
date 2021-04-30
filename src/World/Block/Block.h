#pragma once

#include "../../pch.h"

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
