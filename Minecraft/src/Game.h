#pragma once

#include "World.h"
#include "Player.h"

class Game
{
private:
	GLFWwindow* m_Window;
	std::string m_WindowTitle;
	
	bool m_IsRunning = false;

	World m_World;
	Player m_Player;

public:
	Game();
	~Game();

	bool CreateWindowAndContext(const std::string& title, unsigned int width, unsigned int height);
	void Initialize();
	void StartLoop();

	inline void OnUpdate(double deltaTime);
	inline void OnRender(int width, int height, double fps);

	inline GLFWwindow* GetWindow() const noexcept { return m_Window; }
	inline bool IsRunning() const noexcept { return m_IsRunning; }
	inline World& GetWorld() noexcept { return m_World; }
	inline Player& GetPlayer() noexcept { return m_Player; }
};

extern Game* g_Game;