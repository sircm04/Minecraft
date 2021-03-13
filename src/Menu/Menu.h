#pragma once

class Menu
{
public:
	~Menu()
	{
		std::cout << "Menu was destroyed" << std::endl;
	}

	virtual void Render(int width, int height, int scale, glm::mat4 guiProjection) const = 0;
};