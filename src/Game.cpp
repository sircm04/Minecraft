#include "pch.h"
#include "Game.h"
#include "World/Chunk/Chunk.h"

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

    m_Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minecraft", nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        throw std::exception("failed to create window!");
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::exception("glad failed to initialize!");

    std::cout << glGetString(GL_VERSION) << std::endl;

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(m_Window, (mode->width - WINDOW_WIDTH) * 0.5f, (mode->height - WINDOW_HEIGHT) * 0.5f);
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
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(m_Window);
}
