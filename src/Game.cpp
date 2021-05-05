#include "pch.h"
#include "Game.h"

#include "World/Chunk/Chunk.h"
#include "Utils/Utils.h"

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);

void Game::Run()
{
    Initialize();
    MainLoop();
    Cleanup();
}

void Game::Initialize()
{
    if (!glfwInit())
        throw std::exception("glfw failed to initialize!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minecraft", nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        throw std::exception("failed to create window!");
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::exception("glad failed to initialize!");

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    std::cout << glGetString(GL_VERSION) << std::endl;

    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

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
		static float pitch = glm::degrees(glm::asin(game->m_Camera.front.y)),
			yaw = glm::degrees(std::atan2(game->m_Camera.front.z, game->m_Camera.front.x));

		float xoffset = (float) xpos - lastX;
		float yoffset = lastY - (float) ypos;
		lastX = (float) xpos;
		lastY = (float) ypos;

		xoffset *= game->m_Camera.sensitivity;
		yoffset *= game->m_Camera.sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		pitch = std::clamp(pitch, -89.9f, 89.9f);
		
		game->m_Camera.front = glm::normalize(glm::vec3 {
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		});
	});

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(m_Window, (mode->width - WINDOW_WIDTH) * 0.5f, (mode->height - WINDOW_HEIGHT) * 0.5f);

    auto layout = VertexBufferLayout();
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(1);

    m_WorldVertexArray = std::make_unique<VertexArray>(layout);

    m_WorldShader = std::make_unique<Shader>(std::unordered_map<unsigned int, std::string> {
        { GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/World.vert") },
        { GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/World.frag") }
    });

    constexpr glm::vec3 skyColor = {
        0.49f,
        0.67f,
        1.0f
    };

    glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);

    m_Camera.position = { 8, 52, 8 };

    glEnable(GL_DEPTH_TEST);
}

void Game::MainLoop()
{
    m_Thread = std::thread([&]()
    {
        m_World.Update(ChunkLocation { 0, 0 });
    });

    while (!glfwWindowShouldClose(m_Window))
    {
        Update();
        Render();
    }
}

void Game::Cleanup()
{
    m_Thread.join();

    glfwDestroyWindow(m_Window);

    glfwTerminate();
}

void Game::Update()
{
    glfwPollEvents();
}

void Game::Render()
{
    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);

    if (width == 0 || height == 0)
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = glm::lookAt(m_Camera.position, m_Camera.position + m_Camera.front, m_Camera.up);
    const glm::mat4 projection = glm::perspective(glm::radians(m_Camera.fov), static_cast<float>(width) / static_cast<float>(height), 0.1f, 5000.0f);

    m_WorldVertexArray->Bind();
    m_WorldShader->Bind();
    m_WorldShader->SetMat4("view", view);
    m_WorldShader->SetMat4("projection", projection);
    m_World.RenderChunks(*m_WorldVertexArray);

    glfwSwapBuffers(m_Window);
}

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
