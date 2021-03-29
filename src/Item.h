#pragma once

enum class ItemType : uint8_t
{
	Bread,
	Diamond,

	Count
};

struct Item
{
	ItemType m_ItemType;
};
