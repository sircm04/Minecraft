#pragma once

#include "Menu.h"

class LoadingScreenMenu : public Menu
{
public:
	void Render(int width, int height, int scale, glm::mat4 guiProjection) const;
};