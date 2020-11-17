#include "pch.h"
#include "Player.h"

Player::Player(World* world)
	: Mob(world, glm::vec3(), 10, 10, 9.0f)
{
}

void Player::Input(GLFWwindow* window, double deltaTime)
{
	static uint16_t clickDelay = 0;
	static uint8_t maxBlockTypeKeys = std::min(static_cast<uint8_t>(BlockType::Count), static_cast<uint8_t>(9));
	static glm::vec3 front, newPosition;

	m_Speed = 9.0f;
	front = glm::normalize(glm::vec3 { m_Camera.front.x, 0.0f, m_Camera.front.z });
	newPosition = m_Position;
	
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

		if (blockPosition && !PLAYER_AABB.Intersects({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }, m_Position, *blockPosition))
		{
			glm::ivec2 chunkPosition = m_World->GetChunkPositionFromBlock({ blockPosition->x, blockPosition->z });
			Chunk* chunk = m_World->GetChunk(chunkPosition);
			chunk->SetBlock(m_World->GetBlockPositionInChunk(*blockPosition), { m_BlockInHand });
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
			m_BlockInHand = (BlockType) i;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		m_Speed *= 5;

	m_Speed *= deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		newPosition += m_Speed * front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		newPosition -= m_Speed * front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		newPosition -= glm::normalize(glm::cross(front, m_Camera.up)) * m_Speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		newPosition += glm::normalize(glm::cross(front, m_Camera.up)) * m_Speed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		newPosition += m_Speed * m_Camera.up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		newPosition -= m_Speed * m_Camera.up;

	if (newPosition != m_Position)
	{
		if (!PLAYER_AABB.IntersectsBlocks(m_World, { newPosition.x, m_Position.y, m_Position.z }))
			m_Position.x = newPosition.x;

		if (!PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, m_Position.y, newPosition.z }))
			m_Position.z = newPosition.z;
		
		if (!PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, newPosition.y, m_Position.z }))
			m_Position.y = newPosition.y;
	}

	m_Camera.position = m_Position;
}

void Player::Update(double deltaTime)
{
}

void Player::Render()
{
}

std::optional<glm::vec3> Player::GetTargetBlockPosition(int max, bool place) const
{
	return m_World->GetTargetBlockPosition(m_Position, m_Camera.front, max, place);
}