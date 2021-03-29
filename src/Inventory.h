#pragma once

#include "Item.h"

struct Inventory
{
	std::vector<std::optional<Item>> items;
};
