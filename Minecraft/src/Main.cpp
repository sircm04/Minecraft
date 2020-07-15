#include "pch.h"

#include "Player.h"
#include "Mesh.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ArrayTexture.h"
#include "Texture.h"
#include "World.h"
#include "Chunk.h"

#include "Assets.h"
#include "Utils.h"

std::unique_ptr<World> world = std::make_unique<World>();
std::unique_ptr<Player> player = std::make_unique<Player>(&(*world));

bool firstMouse = true;
float lastX = 0, lastY = 0;
float pitch, yaw = -90;

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwSwapInterval(1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(856, 482, "Minecraft", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	if (!status)
		return -1;

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

	std::cout << glGetString(GL_VERSION) << std::endl;

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
		if (width == 0 || height == 0) return;

		glViewport(0, 0, width, height);
	});

	std::unique_ptr<Shader> shader, entityShader, guiShader;
	VertexBufferLayout guiLayout;
	
	guiLayout.Push<float>(2);
	guiLayout.Push<float>(2);

	std::unique_ptr<Mesh> guiMesh = std::make_unique<Mesh>(std::vector<float> {
		0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f
	}, std::vector<unsigned int> { 0, 1, 2, 3, 4, 5 }, guiLayout);

	shader = std::make_unique<Shader>(std::vector<ShaderEntry> {
		{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Lighting.vert") },
		{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Lighting.frag") }
	});

	entityShader = std::make_unique<Shader>(std::vector<ShaderEntry> {
		{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Basic2dTex.vert") },
		{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Basic2dTex.frag") }
	});

	guiShader = std::make_unique<Shader>(std::vector<ShaderEntry> {
		{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/GUI.vert") },
		{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/GUI.frag") }
	});

	std::vector<std::string> paths = {
		"grass_side.png", "grass_top.png", "grass_bottom.png", "stone.png",
		"cobblestone.png", "bedrock.png", "wood.png", "log_side.png", "log_top.png"
	};

	for (int i = 0; i < paths.size(); i++)
		paths[i] = ("res/images/" + paths[i]);

	std::unique_ptr<ArrayTexture> blockArrayTexture = std::make_unique<ArrayTexture>(paths, 16, 16);
	std::unique_ptr<Texture> heartTexture = std::make_unique<Texture>("res/images/heart.png");
	std::unique_ptr<Texture> dirtTexture = std::make_unique<Texture>("res/images/grass_bottom.png");
	std::unique_ptr<Texture> crosshairTexture = std::make_unique<Texture>("res/images/crosshair.png");

	Assets::InitializeAssets();

	player->m_Camera.front.z = -1.0f;

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
	{
		if (world->m_FirstLoad)
			return;

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

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		player->m_Camera.front = glm::normalize(front);
	});

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float startWidth = static_cast<GLfloat>(width), startHeight = static_cast<GLfloat>(height);

	glm::mat4 guiProjection = glm::ortho(0.0f, startWidth, startHeight, 0.0f, -1.0f, 1.0f);
	float dirtSize = 57.07f;

	glClearColor(0.54117f, 0.64705f, 0.96470f, 1.0f);

	std::unordered_map<glm::ivec2, Chunk> chunks;

	srand(time(0));
	siv::PerlinNoise noise = siv::PerlinNoise(rand() % 1000 + 1);

	std::thread([&]()
	{
		while (!glfwWindowShouldClose(window))
		{
			world->Update(&player->m_Camera.position);
		}
	}).detach();

	float deltaTime = 0.0f,
		lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();

		glfwGetWindowSize(window, &width, &height);

		player->Input(window, deltaTime);

		if (width > 0 && height > 0)
		{
			if (world->m_FirstLoad)
			{
				glClear(GL_DEPTH_BUFFER_BIT);

				guiShader->Bind();
				guiMesh->Bind();
				guiShader->SetMat4("projection", guiProjection);

				dirtTexture->Bind();

				for (int x = 0; x < ((width / dirtSize) + 1); x++)
				{
					for (int y = 0; y < ((height / dirtSize) + 1); y++)
					{
						guiShader->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), { x * dirtSize, y * dirtSize, 0.0f }), { dirtSize, dirtSize, 0.0f }));
						guiShader->SetVec4("u_color", glm::vec4 { 0.4f, 0.4f, 0.4f, 1.0f });
						guiMesh->Render();
					}
				}

				guiShader->SetVec4("u_color", glm::vec4 { 1.0f, 1.0f, 1.0f, 1.0f });
			}
			else
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				shader->Bind();
				blockArrayTexture->Bind();

				glm::mat4 view = glm::lookAt(player->m_Camera.position,
					player->m_Camera.position + player->m_Camera.front, player->m_Camera.up);
				glm::mat4 projection = glm::perspective(glm::radians(60.0f), static_cast<GLfloat>(width) / static_cast<GLfloat>(height), 0.1f, 5000.0f);

				glm::mat4 matrix = glm::mat4(1.0f);
				/*matrix = glm::rotate_slow(glm::translate(matrix,
				{
					2.0f * sin(glfwGetTime()),
					-0.3f,
					1.5f * cos(glfwGetTime())
				}), glm::radians((float)glfwGetTime() * 50.0f), { 1.0f, 1.0f, 1.0f });*/

				shader->SetMat4("view", view);
				shader->SetMat4("projection", projection);
				shader->SetMat4("model", glm::translate(matrix, { 0.0f, 0.0f, 0.0f }));

				shader->SetVec3("lightColor", { 1.0f, 1.0f, 1.0f });
				shader->SetVec3("lightPos", { (Chunk::CHUNK_WIDTH / 2) + 0.5f, (Chunk::CHUNK_HEIGHT / 2) + 40, (Chunk::CHUNK_DEPTH / 2) + 0.5f });
				shader->SetVec3("viewPos", player->m_Camera.position);

				world->RenderChunks();

				entityShader->Bind();
				entityShader->SetMat4("view", view);
				entityShader->SetMat4("projection", projection);

				world->RenderEntities(&(*entityShader));

				glClear(GL_DEPTH_BUFFER_BIT);

				guiShader->Bind();
				guiMesh->Bind();
				guiShader->SetMat4("projection", guiProjection);

				heartTexture->Bind();
				for (int i = 0; i < player->m_Health; i++)
				{
					guiShader->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), { 12.0f + (i * 13.33f), 455.0f, 0.0f }), { 15.0f, 15.0f, 0.0f }));
					guiMesh->Render();
				}

				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
				crosshairTexture->Bind();
				guiShader->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
					{ (startWidth / 2.0f) - 7.5f, (startHeight / 2.0f) - 7.5f, 0.0f }), { 15.0f, 15.0f, 0.0f }));
				guiShader->SetVec4("u_color", glm::vec4 { 1.0f, 1.0f, 1.0f, 1.0f });
				guiMesh->Render();
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				guiShader->SetVec4("u_color", glm::vec4 { 1.0f, 1.0f, 1.0f, 1.0f });
			}
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}