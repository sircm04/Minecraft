#pragma once

#include "World/World.h"
#include "Entity/Mob/Player.h"

class Game
{
private:
	GLFWwindow* m_Window;
	std::string m_WindowTitle;
	
	std::thread m_Thread;
	bool m_IsRunning = false;

	World m_World;
	Player m_Player = Player(&m_World);

public:
	Game() noexcept = default;
	~Game();

	bool CreateWindowAndContext(const std::string& title, unsigned int width, unsigned int height);
	void Initialize();
	void StartLoop();

	void OnUpdate(double deltaTime);
	void OnRender(int width, int height, double fps);

	GLFWwindow* GetWindow() const noexcept { return m_Window; }
	bool IsRunning() const noexcept { return m_IsRunning; }
	World& GetWorld() noexcept { return m_World; }
	Player& GetPlayer() noexcept { return m_Player; }
};

extern Game* g_Game;