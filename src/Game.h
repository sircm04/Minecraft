#pragma once

#include "World/World.h"
#include "Camera.h"
#include "Renderer/Shader.h"

class Game
{
private:
	GLFWwindow* m_Window;
	const uint32_t WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

	World m_World;

	std::thread m_Thread;
	std::unique_ptr<VertexArray> m_WorldVertexArray;
	std::unique_ptr<Shader> m_WorldShader;
	Camera m_Camera;

public:
	~Game();

	void Run();

	void Initialize();
	void MainLoop();
	void Cleanup();

	void Update();
	void Render();
};
