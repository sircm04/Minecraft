#pragma once

#include "World/World.h"
#include "Entity/Mob/Player.h"
#include "Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/ArrayTexture.h"

#define TITLE "Minecraft"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

class Game
{
private:
	static Game* INSTANCE;

public:
	static constexpr inline float GRAVITY = 24.0f;

	GLFWwindow* m_Window;

	World m_World;
	Player m_Player = Player(m_World, { 8.5f, 50.0f, 8.5f });

	std::vector<std::thread> m_Threads;
	std::unique_ptr<Shader> m_WorldShader;
	std::unique_ptr<ArrayTexture> m_WorldTexture;

	Game(int width, int height);
	~Game();

	void Initialize(int width, int height);
	void Run();
	void Cleanup();

	void Update(float deltaTime);
	void Render();

	static Game* Instance() { return INSTANCE; }
};
