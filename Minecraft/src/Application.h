#pragma once

#include "World.h"
#include "Player.h"

class Application
{
private:
	const std::string WINDOW_TITLE = "Minecraft";
	static constexpr unsigned int WINDOW_WIDTH = 856, WINDOW_HEIGHT = 482;

	GLFWwindow* m_Window;
	bool m_IsRunning = false;

	World m_World;
	Player m_Player;

public:
	Application();
	~Application();

	bool CreateContext();
	void Initialize();
	void StartLoop();

	inline void OnUpdate(double deltaTime);
	inline void OnRender(int width, int height);

	inline GLFWwindow* GetWindow() const noexcept { return m_Window; }
	inline bool IsRunning() const noexcept { return m_IsRunning; }
	inline World& GetWorld() noexcept { return m_World; }
	inline Player& GetPlayer() noexcept { return m_Player; }
};

extern Application* g_Application;