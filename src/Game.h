#pragma once

#include "World/World.h"

class Game
{
private:
	GLFWwindow* m_Window;
	const uint32_t WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

	World m_World;

	std::thread m_Thread;

public:
	void Run();

	void Initialize();
	void MainLoop();
	void Cleanup();

	void Update();
	void Render();
};
