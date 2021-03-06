#include "pch.h"
#include "Game.h"

#include "Assets.h"
#include "Renderer/TextRenderer.h"

Game::~Game()
{
	m_Thread.join();
	glfwTerminate();
}

bool Game::CreateWindowAndContext(const std::string& title, unsigned int width, unsigned int height)
{
	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_SAMPLES, 16);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_WindowTitle = title;
	m_Window = glfwCreateWindow(width, height, m_WindowTitle.c_str(), nullptr, nullptr);
	if (!m_Window)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_Window);

	glfwSwapInterval(1);

	const int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	if (!status)
		return false;

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(m_Window, (mode->width - width) * 0.5f, (mode->height - height) * 0.5f);

	std::cout << glGetString(GL_VERSION) << std::endl;
	return true;
}

void Game::Initialize()
{
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
	{
		if (width == 0 || height == 0) return;

		glViewport(0, 0, width, height);
	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
	{
		static float lastX = xpos, lastY = ypos;
		static float pitch = glm::degrees(glm::asin(g_Game->GetPlayer().m_Camera.front.y)),
			yaw = glm::degrees(std::atan2(g_Game->GetPlayer().m_Camera.front.z, g_Game->GetPlayer().m_Camera.front.x));

		float xoffset = (float) xpos - lastX;
		float yoffset = lastY - (float) ypos;
		lastX = (float) xpos;
		lastY = (float) ypos;

		xoffset *= g_Game->GetPlayer().m_Camera.sensitivity;
		yoffset *= g_Game->GetPlayer().m_Camera.sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		pitch = std::clamp(pitch, -89.9f, 89.9f);
		
		g_Game->GetPlayer().m_Camera.front = glm::normalize(glm::vec3 {
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		});
	});

	const glm::vec3 skyColor = {
		0.49f,
		0.67f,
		1.0f
	};

	const glm::vec3 bottomSkyColor = {
		0.67f,
		0.82f,
		1.0f
	};

	glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);

	Assets::InitializeAssets();

	Assets::SHADERS["BLOCK"]->Bind();
	Assets::SHADERS["BLOCK"]->SetVec2("fogDist", { World::REAL_WORLD_RADIUS * 0.75f, World::REAL_WORLD_RADIUS - (16 * 1.5f) });
	Assets::SHADERS["BLOCK"]->SetVec3("fogColor", bottomSkyColor);
}

void Game::StartLoop()
{
	if (m_IsRunning)
		return;
	m_IsRunning = true;

	m_Thread = std::thread([]()
	{
		while (!glfwWindowShouldClose(g_Game->GetWindow()))
		{
			static double deltaTime = 0, lastTime = 0;

			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			g_Game->GetWorld().Update(deltaTime, &g_Game->GetPlayer(), g_Game->GetPlayer().m_Position);
		}
	});

	while (!glfwWindowShouldClose(m_Window))
	{
		static double deltaTime, lastTime, fpsTimeAccumulator, fps;
		static int nbFrames;

		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		fpsTimeAccumulator += deltaTime;
		nbFrames++;
		if (fpsTimeAccumulator >= 1.0) {
			fps = (double) nbFrames / fpsTimeAccumulator;

			std::stringstream ss;
			ss << "Minecraft [" << fps << " FPS]";

			glfwSetWindowTitle(m_Window, ss.str().c_str());

			nbFrames = 0;
			fpsTimeAccumulator = 0;
		}

		OnUpdate(deltaTime);

		static int width, height;
		glfwGetWindowSize(m_Window, &width, &height);

		if (width > 0 && height > 0)
			OnRender(width, height, fps);
	}
}

inline void Game::OnUpdate(double deltaTime)
{
	glfwPollEvents();

	m_Player.Input(m_Window, deltaTime);
	m_Player.Update(deltaTime);
}

inline void Game::OnRender(int width, int height, double fps)
{
	glm::mat4 guiProjection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

	if (m_World.m_FirstLoad)
	{
		glClear(GL_DEPTH_BUFFER_BIT);

		Assets::SHADERS["GUI"]->Bind();
		Assets::SHADERS["GUI"]->SetMat4("projection", guiProjection);
		Assets::MESHES["GUI"]->Bind();

		Assets::TEXTURES["DIRT"]->Bind();

		static constexpr float dirtSize = 57.07f;
		const uint8_t dirtWidth = ((width / dirtSize) + 1),
			dirtHeight = ((height / dirtSize) + 1);

		for (uint8_t x = 0; x < dirtWidth; ++x)
		{
			for (uint8_t y = 0; y < dirtHeight; ++y)
			{
				Assets::SHADERS["GUI"]->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
				{
					x * dirtSize,
					y * dirtSize,
					0.0f
				}),
				{
					dirtSize,
					dirtSize,
					0.0f
				}));
				Assets::SHADERS["GUI"]->SetVec4("u_color", { 0.4f, 0.4f, 0.4f, 1.0f });
				Assets::MESHES["GUI"]->Render();
			}
		}

		Assets::SHADERS["GUI"]->SetVec4("u_color", { 1.0f, 1.0f, 1.0f, 1.0f });
	}
	else
	{
		const glm::mat4 view = glm::lookAt(m_Player.m_Camera.position,
			m_Player.m_Camera.position + m_Player.m_Camera.front, m_Player.m_Camera.up);
		const glm::mat4 projection = glm::perspective(glm::radians(m_Player.m_Camera.fov),
			(static_cast<float>(width) / static_cast<float>(height)), 0.1f, 5000.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_FALSE);

		glDepthFunc(GL_LEQUAL);

		Assets::SHADERS["SKY"]->Bind();
		Assets::SHADERS["SKY"]->SetMat4("view", glm::mat4(glm::mat3(view)));
		Assets::SHADERS["SKY"]->SetMat4("projection", projection);
		Assets::MESHES["SKY"]->Render();

		Assets::SHADERS["SUN"]->Bind();
		Assets::SHADERS["SUN"]->SetMat4("view", view);
		Assets::SHADERS["SUN"]->SetMat4("projection", projection);

		static constexpr float sunSpeed = 0.005f;

		glm::mat4 translation = glm::translate(glm::mat4(1.0f),
		{
			m_Player.m_Position.x + (cos(glfwGetTime() * sunSpeed) * 7.0f),
			m_Player.m_Position.y + 0.275f + (sin(glfwGetTime() * sunSpeed) * 7.0f),
			m_Player.m_Position.z - 0.275f
		});
		glm::vec3 sunPosition = translation * glm::vec4(0, 0, 0, 1);
		glm::mat4 lookAtMatrix = glm::inverse(glm::lookAt(sunPosition, m_Player.m_Position, { 0.0f, 0.0f, 1.0f }));
		glm::mat4 sunMatrix = lookAtMatrix;

		Assets::SHADERS["SUN"]->SetMat4("model", sunMatrix);
		Assets::ARRAY_TEXTURES["SUN"]->Bind();
		Assets::MESHES["SUN"]->Render();

		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

		Assets::SHADERS["BLOCK"]->Bind();
		Assets::ARRAY_TEXTURES["BLOCKS"]->Bind();

		Assets::SHADERS["BLOCK"]->SetMat4("view", view);
		Assets::SHADERS["BLOCK"]->SetMat4("projection", projection);
		Assets::SHADERS["BLOCK"]->SetMat4("model", glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f }));
		Assets::SHADERS["BLOCK"]->SetVec3("playerPosition", m_Player.m_Position);

		static ViewFrustum frustum;
		frustum.Update(projection * view);
		m_World.RenderChunks(frustum);

		Assets::SHADERS["ENTITY"]->Bind();
		Assets::SHADERS["ENTITY"]->SetMat4("view", view);
		Assets::SHADERS["ENTITY"]->SetMat4("projection", projection);

		m_World.RenderEntities();

		glDisable(GL_MULTISAMPLE);
		glDepthMask(GL_FALSE);

		// Adjusts GUI scaling with window resizing:
		static const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		static const float maxScalingWidth = (0.67f * mode->width), maxScalingHeight = (0.93f * (mode->height - 53)); // 1080p maximized (not fullscreen): 1280/960
		unsigned int size = std::max(1.0f, 4.0f * std::min(1.0f,
			std::min(static_cast<float>(width) / maxScalingWidth, (static_cast<float>(height) / maxScalingHeight))));

		Assets::SHADERS["GUI"]->Bind();
		Assets::SHADERS["GUI"]->SetMat4("projection", guiProjection);
		Assets::MESHES["GUI"]->Bind();

		Assets::TEXTURES["HEART"]->Bind();
		for (uint8_t i = 0; i < m_Player.m_Health; ++i)
		{
			Assets::SHADERS["GUI"]->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
			{
				(2.0f * size) + (i * (8.0f * size)),
				height - (11.0f * size),
				0.0f
			}),
			{
				9.0f * size,
				9.0f * size,
				0.0f
			}));
			Assets::MESHES["GUI"]->Render();
		}

		std::stringstream ss;
		ss << "FPS: " << fps << ", POS: " <<
			static_cast<int>(m_Player.m_Position.x) << ", " <<
			static_cast<int>(m_Player.m_Position.y) << ", " <<
			static_cast<int>(m_Player.m_Position.z);

		TextRenderer::Begin();
		TextRenderer::DrawText(ss.str().c_str(), { (2.0f * size), (2.0f * size) }, size);
		TextRenderer::End();

		glEnable(GL_MULTISAMPLE);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

		Assets::MESHES["GUI"]->Bind();

		Assets::TEXTURES["CROSSHAIR"]->Bind();
		glm::vec3 guiPosition = {
			(static_cast<float>(width) * 0.5f) - (4.5f * size),
			(static_cast<float>(height) * 0.5f) - (4.5f * size),
			0.0f
		};
		Assets::SHADERS["GUI"]->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), guiPosition),
		{
			9.0f * size,
			9.0f * size,
			0.0f
		}));
		Assets::MESHES["GUI"]->Render();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_TRUE);
	}

	glfwSwapBuffers(m_Window);
}