#include "pch.h"
#include "Game.h"

#include "Assets.h"
#include "Renderer/TextRenderer.h"

const std::string Game::VERSION = "0.1.3";

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

	glfwSetWindowUserPointer(m_Window, this);

	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
	{
		if (width == 0 || height == 0) return;

		glViewport(0, 0, width, height);
	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
	{
		auto game = static_cast<Game*>(glfwGetWindowUserPointer(window));

		static float lastX = xpos, lastY = ypos;
		static float pitch = glm::degrees(glm::asin(game->GetPlayer().m_Camera.front.y)),
			yaw = glm::degrees(std::atan2(game->GetPlayer().m_Camera.front.z, game->GetPlayer().m_Camera.front.x));

		float xoffset = (float) xpos - lastX;
		float yoffset = lastY - (float) ypos;
		lastX = (float) xpos;
		lastY = (float) ypos;

		xoffset *= game->GetPlayer().m_Camera.sensitivity;
		yoffset *= game->GetPlayer().m_Camera.sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		pitch = std::clamp(pitch, -89.9f, 89.9f);
		
		game->GetPlayer().m_Camera.front = glm::normalize(glm::vec3 {
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

	Assets::SHADERS.at("BLOCK").Bind();
	Assets::SHADERS.at("BLOCK").SetVec2("fogDist", { World::REAL_WORLD_RADIUS * 0.75f, World::REAL_WORLD_RADIUS - (16 * 1.5f) });
	Assets::SHADERS.at("BLOCK").SetVec3("fogColor", bottomSkyColor);

	m_Player.m_Inventory = {
		{
			Item { ItemType::Bread },
			std::nullopt,
			Item { ItemType::Bread },
			Item { ItemType::Bread },
			Item { ItemType::Diamond },
			Item { ItemType::Bread },
			Item { ItemType::Bread }
		}
	};
}

void Game::StartLoop()
{
	if (m_IsRunning)
		return;
	m_IsRunning = true;

	m_Thread = std::thread([&]()
	{
		while (!glfwWindowShouldClose(this->GetWindow()))
		{
			static double deltaTime = 0, lastTime = 0;

			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			this->GetWorld().Update(deltaTime, &this->GetPlayer(), this->GetPlayer().m_Position);
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

	m_IsRunning = false;
}

void Game::OnUpdate(double deltaTime)
{
	glfwPollEvents();

	m_Player.Input(m_Window, deltaTime);
	m_Player.Update(deltaTime);
}

void Game::OnRender(int width, int height, double fps)
{
	glm::mat4 guiProjection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_World.m_FirstLoad)
	{
		// Render dirt block loading screen:

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		Assets::SHADERS.at("GUI").Bind();
		Assets::SHADERS.at("GUI").SetMat4("projection", guiProjection);
		Assets::MESHES.at("GUI").Bind();
		Assets::TEXTURES.at("DIRT").Bind();

		static constexpr float dirtSize = 57.07f;
		const uint8_t dirtWidth = ((width / dirtSize) + 1),
			dirtHeight = ((height / dirtSize) + 1);

		for (uint8_t x = 0; x < dirtWidth; ++x)
		{
			for (uint8_t y = 0; y < dirtHeight; ++y)
			{
				Assets::SHADERS.at("GUI").SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
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
				Assets::SHADERS.at("GUI").SetVec4("u_color", { 0.4f, 0.4f, 0.4f, 1.0f });
				Assets::MESHES.at("GUI").Render();
			}
		}

		Assets::SHADERS.at("GUI").SetVec4("u_color", { 1.0f, 1.0f, 1.0f, 1.0f });

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}
	else
	{
		const glm::mat4 view = glm::lookAt(m_Player.m_Camera.position,
			m_Player.m_Camera.position + m_Player.m_Camera.front, m_Player.m_Camera.up);
		const glm::mat4 projection = glm::perspective(glm::radians(m_Player.m_Camera.fov),
			(static_cast<float>(width) / static_cast<float>(height)), 0.1f, 5000.0f);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		// Render skybox:

		Assets::SHADERS.at("SKY").Bind();
		Assets::SHADERS.at("SKY").SetMat4("view", glm::mat4(glm::mat3(view)));
		Assets::SHADERS.at("SKY").SetMat4("projection", projection);
		Assets::MESHES.at("SKY").Render();

		// Render sun:

		Assets::SHADERS.at("SUN").Bind();
		Assets::SHADERS.at("SUN").SetMat4("view", view);
		Assets::SHADERS.at("SUN").SetMat4("projection", projection);

		static constexpr float sunSpeed = 0.005f;
		glm::mat4 translation = glm::translate(glm::mat4(1.0f),
		{
			m_Player.m_Position.x + (cos(glfwGetTime() * sunSpeed) * 10.0f),
			m_Player.m_Position.y + (sin(glfwGetTime() * sunSpeed) * 10.0f),
			m_Player.m_Position.z
		});
		glm::vec3 sunPosition = translation * glm::vec4(0, 0, 0, 1);
		glm::mat4 lookAtMatrix = glm::inverse(glm::lookAt(sunPosition, m_Player.m_Position, { 0.0f, 0.0f, 1.0f }));
		glm::mat4 sunMatrix = lookAtMatrix;

		Assets::SHADERS.at("SUN").SetMat4("model", sunMatrix);
		Assets::TEXTURES.at("SUN").Bind();
		Assets::MESHES.at("SUN").Render();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

		// Render world:

		Assets::SHADERS.at("BLOCK").Bind();
		Assets::ARRAY_TEXTURES.at("BLOCKS").Bind();

		Assets::SHADERS.at("BLOCK").SetMat4("view", view);
		Assets::SHADERS.at("BLOCK").SetMat4("projection", projection);
		Assets::SHADERS.at("BLOCK").SetMat4("model", glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f }));
		Assets::SHADERS.at("BLOCK").SetVec3("playerPosition", m_Player.m_Position);

		glDisable(GL_BLEND);

		static ViewFrustum frustum;
		frustum.Update(projection * view);
		m_World.RenderChunks(frustum);

		glEnable(GL_BLEND);

		// Render entities:

		Assets::SHADERS.at("ENTITY").Bind();
		Assets::SHADERS.at("ENTITY").SetMat4("view", view);
		Assets::SHADERS.at("ENTITY").SetMat4("projection", projection);

		m_World.RenderEntities();

		// Render target block hitbox:

		std::optional<glm::ivec3> targetBlockPosition = m_Player.GetTargetBlockPosition(10);

		if (targetBlockPosition)
		{
			glDepthFunc(GL_LEQUAL);
			glLineWidth(2.0f);

			Assets::SHADERS.at("BLOCK_HITBOX").Bind();
			Assets::SHADERS.at("BLOCK_HITBOX").SetMat4("view", view);
			Assets::SHADERS.at("BLOCK_HITBOX").SetMat4("projection", projection);
			Assets::SHADERS.at("BLOCK_HITBOX").SetMat4("model", glm::translate(glm::mat4(1.0f), static_cast<glm::vec3>(*targetBlockPosition)));
			Assets::MESHES.at("BLOCK_HITBOX").Bind();
			Assets::MESHES.at("BLOCK_HITBOX").Render(GL_LINES);
			glDepthFunc(GL_LESS);
		}

		glDisable(GL_MULTISAMPLE);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		// Adjusts GUI scaling with window resizing:
		static const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		static const float maxScalingWidth = (0.67f * mode->width), maxScalingHeight = (0.93f * (mode->height - 53)); // 1080p maximized (not fullscreen): 1280/960
		const uint8_t scale = std::max(1.0f, 4.0f * std::min(1.0f,
			std::min(static_cast<float>(width) / maxScalingWidth, (static_cast<float>(height) / maxScalingHeight))));

		// Render hearts:

		Assets::SHADERS.at("GUI").Bind();
		Assets::SHADERS.at("GUI").SetMat4("projection", guiProjection);
		Assets::MESHES.at("GUI").Bind();

		Assets::TEXTURES.at("HEART").Bind();
		for (uint8_t i = 0; i < m_Player.m_Health; ++i)
		{
			Assets::SHADERS.at("GUI").SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
			{
				(2.0f * scale) + (i * (8.0f * scale)),
				height - (11.0f * scale),
				0.0f
			}),
			{
				9.0f * scale,
				9.0f * scale,
				0.0f
			}));
			Assets::MESHES.at("GUI").Render();
		}

		// Render text:

		TextRenderer::Begin();
		TextRenderer::DrawText("sircm04's Minecraft Clone (v" + VERSION + ")", { (2.0f * scale), (2.0f * scale) }, scale);
		TextRenderer::DrawText(std::to_string(static_cast<int>(fps)) + " fps", { (2.0f * scale), (11.0f * scale) }, scale);
		TextRenderer::DrawText("Position: "
			+ std::to_string(static_cast<int>(m_Player.m_Position.x)) + " "
			+ std::to_string(static_cast<int>(m_Player.m_Position.y)) + " "
			+ std::to_string(static_cast<int>(m_Player.m_Position.z)),
			{ (2.0f * scale), (20.0f * scale) }, scale);
		const glm::ivec3 direction = glm::round(m_Player.m_Camera.front);
		TextRenderer::DrawText("Direction: " + std::to_string(direction.x) + " " + std::to_string(direction.y) + " " + std::to_string(direction.z), { (2.0f * scale), (29.0f * scale) }, scale);
		TextRenderer::DrawText("Day: " + std::to_string(static_cast<int>((glfwGetTime() * sunSpeed) / 6.283185f) + 1), { (2.0f * scale), (38.0f * scale) }, scale);
		TextRenderer::DrawText("Block In Hand: " + GetBlockTypeData(m_Player.m_BlockInHand).name, { (2.0f * scale), (47.0f * scale) }, scale);
		std::string isFlying = "Is Flying: ";
		isFlying.append(m_Player.m_IsFlying ? "true" : "false");
		TextRenderer::DrawText(isFlying, { (2.0f * scale), (55.0f * scale) }, scale);
		TextRenderer::End();

		// Render crosshair:

		glEnable(GL_MULTISAMPLE);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

		Assets::MESHES.at("GUI").Bind();
		Assets::TEXTURES.at("CROSSHAIR").Bind();
		Assets::SHADERS.at("GUI").SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
		{
			(static_cast<float>(width) * 0.5f) - (4.5f * scale),
			(static_cast<float>(height) * 0.5f) - (4.5f * scale),
			0.0f
		}),
		{
			9.0f * scale,
			9.0f * scale,
			0.0f
		}));
		Assets::MESHES.at("GUI").Render();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Render hotbar:

		Assets::SHADERS.at("GUI_ARRAY").Bind();
		Assets::SHADERS.at("GUI_ARRAY").SetMat4("projection", guiProjection);
		Assets::MESHES.at("GUI").Bind();
		Assets::ARRAY_TEXTURES.at("ITEMS").Bind();
		Assets::SHADERS.at("GUI_ARRAY").SetVec4("u_color", { 1.0f, 1.0f, 1.0f, 1.0f });

		const int hotbarWidth = m_Player.m_Inventory.items.size();
		for (int i = 0; i < m_Player.m_Inventory.items.size(); ++i)
		{
			auto item = m_Player.m_Inventory.items[i];

			if (item)
			{
				Assets::SHADERS.at("GUI_ARRAY").SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
				{
					(width * 0.5f) + (i - (hotbarWidth * 0.5f)) * (16.0f * scale),
					height - (16.0f * scale),
					0.0f
				}),
				{
					(16.0f * scale),
					(16.0f * scale),
					0.0f
				}));
				Assets::SHADERS.at("GUI_ARRAY").SetFloat("texZ", static_cast<float>(item->m_ItemType));
				Assets::MESHES.at("GUI").Render();
			}
		}

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}

	glfwSwapBuffers(m_Window);
}
