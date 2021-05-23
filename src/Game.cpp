#include "pch.h"
#include "Game.h"

#include "World/Chunk/Chunk.h"
#include "Utils/Utils.h"
#include "Math/Frustum.h"

#if _DEBUG
void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);
#endif

Game::Game(int width, int height)
{
    Initialize(width, height);
    Run();
}

Game::~Game()
{
    Cleanup();
}

void Game::Initialize(int width, int height)
{
    if (!glfwInit())
        throw std::exception("glfw failed to initialize!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 16);

    m_Window = glfwCreateWindow(width, height, "Minecraft", nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        throw std::runtime_error("failed to create window!");
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("glad failed to initialize!");

#if _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    std::cout << glGetString(GL_VERSION) << std::endl;

    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowUserPointer(m_Window, this);

	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
	{
		if (width == 0 || height == 0)
            return;

		glViewport(0, 0, width, height);
	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
	{
		auto game = static_cast<Game*>(glfwGetWindowUserPointer(window));

		static float lastX = xpos, lastY = ypos;
		static float pitch = glm::degrees(glm::asin(game->m_Player.m_Camera.front.y)),
			yaw = glm::degrees(std::atan2(game->m_Player.m_Camera.front.z, game->m_Player.m_Camera.front.x));

		float xoffset = (float) xpos - lastX;
		float yoffset = lastY - (float) ypos;
		lastX = (float) xpos;
		lastY = (float) ypos;

		xoffset *= game->m_Player.m_Camera.sensitivity;
		yoffset *= game->m_Player.m_Camera.sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		pitch = std::clamp(pitch, -89.9f, 89.9f);
		
		game->m_Player.m_Camera.front = glm::normalize(glm::vec3 {
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		});
	});

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(m_Window, (mode->width - width) * 0.5f, (mode->height - height) * 0.5f);

    m_WorldShader = std::make_unique<Shader>(std::unordered_map<unsigned int, std::string> {
        { GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/World.vert") },
        { GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/World.frag") }
    });

    m_WorldTexture = std::make_unique<ArrayTexture>(std::vector<std::string>
    {
        "res/images/grass_side.png", "res/images/grass_top.png", "res/images/dirt.png",
		"res/images/stone.png", "res/images/cobblestone.png", "res/images/bedrock.png",
		"res/images/wood.png", "res/images/log_side.png", "res/images/log_top.png",
		"res/images/leaves.png", "res/images/glass.png"
    }, 16, 16);

    constexpr glm::vec3 skyColor = {
        0.49f,
        0.67f,
        1.0f
    };

    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);

    m_Player.m_Position = { 8.5f, 52, 8.5f };

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
}

void Game::Run()
{
    m_Thread = std::thread([&]()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            m_World.Update({ m_Player.m_Position.x, m_Player.m_Position.z });
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

        Update(deltaTime);
        Render();
    }
}

void Game::Cleanup()
{
    m_Thread.join();

    glfwDestroyWindow(m_Window);

    glfwTerminate();
}

void Game::Update(float deltaTime)
{
    glfwPollEvents();

    m_Player.Input(m_Window, deltaTime);
    m_Player.Update(deltaTime);
}

void Game::Render()
{
    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);

    if (width == 0 || height == 0)
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 bobbingOffset = { 0.0f, 0.0f, 0.0f };
    float bobbingHorizontal = -sin(m_Player.m_NumSteps) * 0.075f;
    if (static_cast<uint8_t>(m_Player.m_WalkingDirection) < 2)
        bobbingOffset.x = bobbingHorizontal;
    else
        bobbingOffset.y = bobbingHorizontal;

    glm::mat4 bobbingMatrix = glm::inverse(glm::rotate(glm::translate(glm::mat4(1.0f), bobbingOffset),
        (-abs(sin(m_Player.m_NumSteps)) * 2 + 1.0f) * 0.005f, { 1.0f, 0.0f, 0.0f }));

    const glm::mat4 view = bobbingMatrix * glm::lookAt(m_Player.m_Position, m_Player.m_Position + m_Player.m_Camera.front, m_Player.m_Camera.up);
    const glm::mat4 projection = glm::perspective(glm::radians(m_Player.m_Camera.fov), static_cast<float>(width) / static_cast<float>(height), 0.1f, 5000.0f);

    static ViewFrustum frustum;
    frustum.Update(projection * view);

    m_WorldTexture->Bind();
    m_WorldShader->Bind();
    m_WorldShader->SetMat4("view", view);
    m_WorldShader->SetMat4("projection", projection);
    m_World.RenderChunks(frustum);

    glfwSwapBuffers(m_Window);
}

#if _DEBUG
void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}
#endif
