#include "pch.h"
#include "Application.h"

#include "Assets.h"

Application::Application()
	: m_World(), m_Player(&m_World)
{
}

Application::~Application()
{
	glfwTerminate();
}

bool Application::CreateContext()
{
	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_SAMPLES, 16);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	constexpr unsigned int windowWidth = 856, windowHeight = 482;
	m_Window = glfwCreateWindow(windowWidth, windowHeight, WINDOW_TITLE.c_str(), nullptr, nullptr);
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

	std::cout << glGetString(GL_VERSION) << std::endl;
	return true;
}

void Application::Initialize()
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
		static bool firstMouse = true;
		static float lastX = 0, lastY = 0;
		static float pitch, yaw = -90;

		if (firstMouse)
		{
			lastX = (float) xpos;
			lastY = (float) ypos;
			firstMouse = false;
		}

		float xoffset = (float) xpos - lastX;
		float yoffset = lastY - (float) ypos;
		lastX = (float) xpos;
		lastY = (float) ypos;

		float sensitivity = 0.25;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		g_Application->GetPlayer().m_Camera.front = glm::normalize(glm::vec3 {
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		});
	});

	glClearColor(0.54117f, 0.64705f, 0.96470f, 1.0f);

	Assets::InitializeAssets();

	Assets::SHADER->Bind();
	Assets::SHADER->SetVec3("lightColor", { 1.0f, 1.0f, 1.0f });
	Assets::SHADER->SetVec2("fogDist", glm::vec2 { World::REAL_WORLD_RADIUS * 0.75, World::REAL_WORLD_RADIUS - (16 * 2) });
	Assets::SHADER->SetVec3("fogColor", glm::vec3 { 0.54117f, 0.64705f, 0.96470f });
}

void Application::StartLoop()
{
	if (m_IsRunning)
		return;
	m_IsRunning = true;

	std::thread([]()
	{
		while (!glfwWindowShouldClose(g_Application->GetWindow()))
		{
			static double deltaTime = 0, lastTime = 0;

			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			g_Application->GetWorld().Update(deltaTime, &g_Application->GetPlayer(), g_Application->GetPlayer().m_Position);
			g_Application->GetPlayer().Update(deltaTime);
		}
	}).detach();

	while (!glfwWindowShouldClose(m_Window))
	{
		static double deltaTime = 0, lastTime = 0, fpsTimeAccumulator = 0;
		static int nbFrames = 0;

		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		fpsTimeAccumulator += deltaTime;
		nbFrames++;
		if (fpsTimeAccumulator >= 1.0) {
			double fps = (double) nbFrames / fpsTimeAccumulator;

			std::stringstream ss;
			ss << "Minecraft [" << fps << " FPS]";

			glfwSetWindowTitle(m_Window, ss.str().c_str());

			nbFrames = 0;
			fpsTimeAccumulator = 0;
		}

		glfwPollEvents();

		OnUpdate(deltaTime);

		static int width, height;
		glfwGetWindowSize(m_Window, &width, &height);

		if (width > 0 && height > 0)
			OnRender(width, height);

		glfwSwapBuffers(m_Window);
	}
}

inline void Application::OnUpdate(double deltaTime)
{
	m_Player.Input(m_Window, deltaTime);
}

inline void Application::OnRender(int width, int height)
{
	const glm::mat4 guiProjection = glm::ortho(0.0f, (float) WINDOW_WIDTH, (float) WINDOW_HEIGHT, 0.0f, -1.0f, 1.0f);

	if (m_World.m_FirstLoad)
	{
		glClear(GL_DEPTH_BUFFER_BIT);

		Assets::GUI_SHADER->Bind();
		Assets::GUI_MESH->Bind();
		Assets::GUI_SHADER->SetMat4("projection", guiProjection);

		Assets::DIRT_TEXTURE->Bind();

		static constexpr float dirtSize = 57.07f;
		const uint8_t dirtWidth = ((width / dirtSize) + 1),
			dirtHeight = ((height / dirtSize) + 1);

		for (uint8_t x = 0; x < dirtWidth; ++x)
		{
			for (uint8_t y = 0; y < dirtHeight; ++y)
			{
				Assets::GUI_SHADER->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), { x * dirtSize, y * dirtSize, 0.0f }), { dirtSize, dirtSize, 0.0f }));
				Assets::GUI_SHADER->SetVec4("u_color", glm::vec4{ 0.4f, 0.4f, 0.4f, 1.0f });
				Assets::GUI_MESH->Render();
			}
		}

		Assets::GUI_SHADER->SetVec4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const glm::mat4 view = glm::lookAt(m_Player.m_Camera.position,
			m_Player.m_Camera.position + m_Player.m_Camera.front, m_Player.m_Camera.up);
		const glm::mat4 projection = glm::perspective(glm::radians(60.0f), ((float) width / (float) height), 0.1f, 5000.0f);

		Assets::SUN_SHADER->Bind();

		Assets::SUN_SHADER->SetMat4("view", view);
		Assets::SUN_SHADER->SetMat4("projection", projection);

		static constexpr float sunSpeed = 0.005f;

		glm::mat4 matrix = glm::translate(glm::mat4(1.0f), m_Player.m_Camera.position - glm::vec3{ 0.5f, 0.5f, 0.5f });

		matrix = glm::rotate(glm::translate(matrix,
			{
				0,
				10.0f * sin(glfwGetTime() * sunSpeed),
				7.5f * cos(glfwGetTime() * sunSpeed)
			}), glm::radians((float)glfwGetTime() * (sunSpeed / 0.017f)), { -1.0f, 0.0f, 0.0f });

		Assets::SUN_SHADER->SetMat4("model", matrix);

		Assets::SUN_TEXTURE->Bind();
		Assets::SUN_MESH->Render();

		glClear(GL_DEPTH_BUFFER_BIT);

		Assets::SHADER->Bind();
		Assets::BLOCK_ARRAY_TEXTURE->Bind();

		Assets::SHADER->SetMat4("view", view);
		Assets::SHADER->SetMat4("projection", projection);
		Assets::SHADER->SetMat4("model", glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f }));

		//Assets::SHADER->SetVec3("lightPos", { (Chunk::CHUNK_WIDTH / 2) + 0.5f, (Chunk::CHUNK_HEIGHT / 2) + 40, (Chunk::CHUNK_DEPTH / 2) + 0.5f });
		//Assets::SHADER->SetVec3("viewPos", player.m_Camera.position);

		Assets::SHADER->SetVec3("playerPosition", m_Player.m_Position);

		static ViewFrustum frustum;
		frustum.Update(projection * view);
		m_World.RenderChunks(frustum, m_Player.m_Position);

		Assets::ENTITY_SHADER->Bind();
		Assets::ENTITY_SHADER->SetMat4("view", view);
		Assets::ENTITY_SHADER->SetMat4("projection", projection);

		m_World.RenderEntities();

		glClear(GL_DEPTH_BUFFER_BIT);

		Assets::GUI_SHADER->Bind();
		Assets::GUI_MESH->Bind();
		Assets::GUI_SHADER->SetMat4("projection", guiProjection);

		Assets::HEART_TEXTURE->Bind();
		for (uint8_t i = 0; i < m_Player.m_Health; ++i)
		{
			Assets::GUI_SHADER->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), { 12.0f + (i * 13.33f), 455.0f, 0.0f }), { 15.0f, 15.0f, 0.0f }));
			Assets::GUI_MESH->Render();
		}

		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
		Assets::CROSSHAIR_TEXTURE->Bind();
		static constexpr glm::vec3 guiPosition = { (WINDOW_WIDTH / 2.0f) - 7.5f, (WINDOW_HEIGHT / 2.0f) - 7.5f, 0.0f };
		Assets::GUI_SHADER->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
			guiPosition), { 15.0f, 15.0f, 0.0f }));
		Assets::GUI_MESH->Render();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}