#include "pch.h"
#include "Player.h"

const AABB Player::PLAYER_AABB = AABB({ -0.25f, -1.5f, -0.25f }, { 0.25f, 0.25f, 0.25f });

BlockType blockType = (BlockType) 1;

float cameraSpeed;
int clickDelay, maxBlockTypeKeys = std::min((int) BlockType::Count, 9);
glm::vec3 front, newPosition;

Player::Player(World* world) // TODO: Synchronize player-entity position
	: Mob({ (Chunk::CHUNK_WIDTH / 2) + 0.5f, (Chunk::CHUNK_HEIGHT / 2) + 2, (Chunk::CHUNK_DEPTH / 2) + 0.5f }, 9, 9), m_World(world)
{
	m_Camera.position = this->m_Position;
}

void Player::Input(GLFWwindow* window, double deltaTime)
{
	cameraSpeed = 9.0f;
	front = glm::normalize(glm::vec3{ m_Camera.front.x, 0.0f, m_Camera.front.z });
	newPosition = m_Camera.position;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && clickDelay == 0)
	{
		std::optional<glm::ivec3> blockPosition = GetTargetBlockPosition(10);

		if (blockPosition)
		{
			glm::ivec2 chunkPosition = m_World->GetChunkPositionFromBlock({ blockPosition->x, blockPosition->z });
			Chunk* chunk = m_World->GetChunk(chunkPosition);
			chunk->SetBlock(m_World->GetBlockPositionInChunk(*blockPosition), { BlockType::Air });
			chunk->GenerateMesh(m_World, chunkPosition);
			m_World->RefreshNeighboringChunks(*blockPosition);
			clickDelay = 800 * (float) deltaTime;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && clickDelay == 0)
	{
		std::optional<glm::ivec3> blockPosition = GetTargetBlockPosition(10, true);

		if (blockPosition && !PLAYER_AABB.Intersects({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }, m_Camera.position, *blockPosition))
		{
			glm::ivec2 chunkPosition = m_World->GetChunkPositionFromBlock({ blockPosition->x, blockPosition->z });
			Chunk* chunk = m_World->GetChunk(chunkPosition);
			chunk->SetBlock(m_World->GetBlockPositionInChunk(*blockPosition), { blockType });
			chunk->GenerateMesh(m_World, chunkPosition);
			m_World->RefreshNeighboringChunks(*blockPosition);
			clickDelay = 800 * (float) deltaTime;
		}
	}

	if (clickDelay > 0)
		clickDelay--;
	if (clickDelay < 0)
		clickDelay = 0;

	for (int i = 1; i < maxBlockTypeKeys; ++i)
	{
		if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS)
			blockType = (BlockType) i;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cameraSpeed *= 5;

	cameraSpeed *= deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		newPosition += cameraSpeed * front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		newPosition -= cameraSpeed * front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		newPosition -= glm::normalize(glm::cross(front, m_Camera.up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		newPosition += glm::normalize(glm::cross(front, m_Camera.up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		newPosition += cameraSpeed * m_Camera.up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		newPosition -= cameraSpeed * m_Camera.up;

	if (newPosition != m_Camera.position)
	{
		if (!TestCollision({ newPosition.x, m_Camera.position.y, m_Camera.position.z }))
			m_Camera.position.x = newPosition.x;

		if (!TestCollision({ m_Camera.position.x, m_Camera.position.y, newPosition.z }))
			m_Camera.position.z = newPosition.z;
		
		if (!TestCollision({ m_Camera.position.x, newPosition.y, m_Camera.position.z }))
			m_Camera.position.y = newPosition.y;
	}
}

void Player::Update(double deltaTime)
{
}

void Player::Render(Shader* shader)
{
}

bool Player::TestCollision(const glm::vec3& position)
{
	glm::vec3 blockMin = PLAYER_AABB.GetRelativeMinimum(position), blockMax = PLAYER_AABB.GetRelativeMaximum(position);

	blockMin = {
		floor(blockMin.x),
		floor(blockMin.y),
		floor(blockMin.z)
	};

	blockMax = {
		floor(blockMax.x),
		floor(blockMax.y),
		floor(blockMax.z)
	};

	for (int x = blockMin.x; x <= blockMax.x; ++x) {
		for (int y = blockMin.y; y <= blockMax.y; ++y) {
			for (int z = blockMin.z; z <= blockMax.z; ++z) {
				const Block* block = m_World->GetBlock({ x, y, z });

				if (block && block->GetBlockTypeData()->isSolid)
					return true;
			}
		}
	}

	return false;
}

std::optional<glm::vec3> Player::GetTargetBlockPosition(int max, bool place)
{
	return m_World->GetTargetBlockPosition(m_Camera.position, m_Camera.front, max, place);
}