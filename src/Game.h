#pragma once

#include "World/World.h"
#include "Entity/Mob/Player.h"
#include "Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/ArrayTexture.h"

class Game
{
private:
	static Game* INSTANCE;

public:
	static constexpr inline float GRAVITY = 24.0f;

	GLFWwindow* m_Window;

	World m_World;
	Player m_Player = Player(m_World);

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
