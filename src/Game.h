#pragma once

#include "World/World.h"
#include "Entity/Mob/Player.h"
#include "Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/ArrayTexture.h"

class Game
{
private:
	GLFWwindow* m_Window;
	const uint32_t WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

	World m_World;
	Player m_Player = Player(m_World);

	std::thread m_Thread;
	std::unique_ptr<Shader> m_WorldShader;
	std::unique_ptr<ArrayTexture> m_WorldTexture;

public:
	~Game();

	void Run();

	void Initialize();
	void MainLoop();
	void Cleanup();

	void Update(float deltaTime);
	void Render();
};
