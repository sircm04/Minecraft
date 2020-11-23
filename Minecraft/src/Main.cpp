#include "pch.h"

#include "Player.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "World.h"
#include "Chunk.h"

#include "Assets.h"

#include "Frustum.h"

World world = World();
Player player = Player(&world);

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_SAMPLES, 16);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(856, 482, "Minecraft", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	if (!status)
		return -1;

	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
		if (width == 0 || height == 0) return;

		glViewport(0, 0, width, height);
	});

	Assets::InitializeAssets();

	player.m_Camera.front.z = -1.0f;

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
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

		player.m_Camera.front = glm::normalize(glm::vec3 {
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		});
	});

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float startWidth = static_cast<GLfloat>(width), startHeight = static_cast<GLfloat>(height);

	glm::mat4 guiProjection = glm::ortho(0.0f, startWidth, startHeight, 0.0f, -1.0f, 1.0f);
	float dirtSize = 57.07f;
	
	std::thread([window]()
	{
		while (!glfwWindowShouldClose(window))
		{
			static double deltaTime = 0, lastTime = 0;
			
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			world.Update(deltaTime, &player, player.m_Position);
			player.Update(deltaTime);
		}
	}).detach();

	glClearColor(0.54117f, 0.64705f, 0.96470f, 1.0f);

	while (!glfwWindowShouldClose(window))
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

			glfwSetWindowTitle(window, ss.str().c_str());

			nbFrames = 0;
			fpsTimeAccumulator = 0;
		}

		glfwPollEvents();

		glfwGetWindowSize(window, &width, &height);

		player.Input(window, deltaTime);

		if (width > 0 && height > 0)
		{
			if (world.m_FirstLoad)
			{
				glClear(GL_DEPTH_BUFFER_BIT);

				Assets::GUI_SHADER->Bind();
				Assets::GUI_MESH->Bind();
				Assets::GUI_SHADER->SetMat4("projection", guiProjection);

				Assets::DIRT_TEXTURE->Bind();

				for (int x = 0; x < ((width / dirtSize) + 1); x++)
				{
					for (int y = 0; y < ((height / dirtSize) + 1); y++)
					{
						Assets::GUI_SHADER->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), { x * dirtSize, y * dirtSize, 0.0f }), { dirtSize, dirtSize, 0.0f }));
						Assets::GUI_SHADER->SetVec4("u_color", glm::vec4 { 0.4f, 0.4f, 0.4f, 1.0f });
						Assets::GUI_MESH->Render();
					}
				}

				Assets::GUI_SHADER->SetVec4("u_color", glm::vec4 { 1.0f, 1.0f, 1.0f, 1.0f });
			}
			else
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glm::mat4 view = glm::lookAt(player.m_Camera.position,
					player.m_Camera.position + player.m_Camera.front, player.m_Camera.up);
				glm::mat4 projection = glm::perspective(glm::radians(60.0f), static_cast<GLfloat>(width) / static_cast<GLfloat>(height), 0.1f, 5000.0f);

				Assets::SUN_SHADER->Bind();

				Assets::SUN_SHADER->SetMat4("view", view);
				Assets::SUN_SHADER->SetMat4("projection", projection);

				float sunSpeed = 0.005f;

				glm::mat4 matrix = glm::translate(glm::mat4(1.0f), player.m_Camera.position - glm::vec3 { 0.5f, 0.5f, 0.5f });

				matrix = glm::rotate(glm::translate(matrix,
				{
					0,
					10.0f * sin(glfwGetTime() * sunSpeed),
					7.5f * cos(glfwGetTime() * sunSpeed)
				}), glm::radians((float) glfwGetTime() * (sunSpeed / 0.017f)), { -1.0f, 0.0f, 0.0f });

				Assets::SUN_SHADER->SetMat4("model", matrix);
				
				Assets::SUN_TEXTURE->Bind();
				Assets::SUN_MESH->Render();

				glClear(GL_DEPTH_BUFFER_BIT);

				Assets::SHADER->Bind();
				Assets::BLOCK_ARRAY_TEXTURE->Bind();

				Assets::SHADER->SetMat4("view", view);
				Assets::SHADER->SetMat4("projection", projection);
				Assets::SHADER->SetMat4("model", glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f }));

				Assets::SHADER->SetVec3("lightColor", { 1.0f, 1.0f, 1.0f });
				//Assets::SHADER->SetVec3("lightPos", { (Chunk::CHUNK_WIDTH / 2) + 0.5f, (Chunk::CHUNK_HEIGHT / 2) + 40, (Chunk::CHUNK_DEPTH / 2) + 0.5f });
				//Assets::SHADER->SetVec3("viewPos", player.m_Camera.position);

				ViewFrustum frustum;
				frustum.Update(projection * view);
				world.RenderChunks(frustum, player.m_Position);

				Assets::ENTITY_SHADER->Bind();
				Assets::ENTITY_SHADER->SetMat4("view", view);
				Assets::ENTITY_SHADER->SetMat4("projection", projection);

				world.RenderEntities();

				glClear(GL_DEPTH_BUFFER_BIT);

				Assets::GUI_SHADER->Bind();
				Assets::GUI_MESH->Bind();
				Assets::GUI_SHADER->SetMat4("projection", guiProjection);

				Assets::HEART_TEXTURE->Bind();
				for (int i = 0; i < player.m_Health; i++)
				{
					Assets::GUI_SHADER->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), { 12.0f + (i * 13.33f), 455.0f, 0.0f }), { 15.0f, 15.0f, 0.0f }));
					Assets::GUI_MESH->Render();
				}

				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
				Assets::CROSSHAIR_TEXTURE->Bind();
				Assets::GUI_SHADER->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
					{ (startWidth / 2.0f) - 7.5f, (startHeight / 2.0f) - 7.5f, 0.0f }), { 15.0f, 15.0f, 0.0f }));
				Assets::GUI_MESH->Render();
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}