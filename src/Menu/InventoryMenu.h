#pragma once

#include "Menu.h"
#include "../Inventory.h"

class InventoryMenu : public Menu
{
public:
	Inventory* m_Inventory;

	InventoryMenu(Inventory* inventory)
		: m_Inventory(inventory)
	{
	}

	void Render(int width, int height, int scale, glm::mat4 guiProjection) const;
};