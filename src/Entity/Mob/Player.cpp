#include "../../pch.h"
#include "Player.h"

Player::Player(World* world) noexcept
	: Mob(world, glm::vec3(), 10, 10, 9.0f)
{
	
}

void Player::Input(GLFWwindow* window, double deltaTime)
{
	static float clickDelay[3];
	static constexpr uint8_t maxBlockTypeKeys = std::min(static_cast<uint8_t>(BlockType::Count), static_cast<uint8_t>(9));
	static glm::vec3 front, newPosition;
	
	m_Speed = ((m_IsFlying) ? 9.0f : 5.5f);
	front = glm::normalize(glm::vec3 { m_Camera.front.x, 0.0f, m_Camera.front.z });
	newPosition = m_Position;
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && clickDelay[0] == 0)
	{
		std::optional<glm::ivec3> blockPosition = GetTargetBlockPosition(10);

		if (blockPosition)
		{
			const glm::ivec2 chunkPosition = m_World->GetChunkPositionFromBlock({ blockPosition->x, blockPosition->z });
			Chunk* chunk = m_World->GetChunk(chunkPosition);
			chunk->SetBlock(m_World->GetBlockPositionInChunk(*blockPosition), { BlockType::Air });
			chunk->GenerateMesh(m_World, chunkPosition);
			m_World->RefreshNeighboringChunks(*blockPosition);
			clickDelay[0] = 0.2f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && clickDelay[0] == 0)
	{
		std::optional<glm::ivec3> blockPosition = GetTargetBlockPosition(10, true);

		if (blockPosition && !PLAYER_AABB.Intersects({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }, m_Position, *blockPosition))
		{
			const glm::ivec2 chunkPosition = m_World->GetChunkPositionFromBlock({ blockPosition->x, blockPosition->z });
			Chunk* chunk = m_World->GetChunk(chunkPosition);
			chunk->SetBlock(m_World->GetBlockPositionInChunk(*blockPosition), { m_BlockInHand });
			chunk->GenerateMesh(m_World, chunkPosition);
			m_World->RefreshNeighboringChunks(*blockPosition);
			clickDelay[0] = 0.2f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_T) && clickDelay[1] == 0)
	{
		m_Inventory.items.emplace_back(Item{ ItemType::Diamond });
		clickDelay[1] = 0.2f;
	}

	if (glfwGetKey(window, GLFW_KEY_BACKSPACE) && clickDelay[2] == 0)
	{
		m_Inventory.items.pop_back();
		clickDelay[2] = 0.2f;
	}

	for (auto& delay : clickDelay) {
		if (delay > 0)
			delay -= 1 * deltaTime;
		if (delay < 0)
			delay = 0;
	}

	for (uint8_t i = 1; i < maxBlockTypeKeys; ++i)
		if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS)
			m_BlockInHand = static_cast<BlockType>(i);

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		m_Speed *= ((m_IsFlying) ? 5.0f : 1.5f);

	m_Speed *= deltaTime;

	static bool pressed = false;
	if (glfwGetKey(window, GLFW_KEY_F) != GLFW_PRESS)
		pressed = false;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !pressed)
	{
		m_IsFlying = !m_IsFlying;
		if (m_IsFlying)
			m_VelocityY = 0.0f;
		pressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		newPosition += m_Speed * front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		newPosition -= m_Speed * front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		newPosition -= glm::normalize(glm::cross(front, m_Camera.up)) * m_Speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		newPosition += glm::normalize(glm::cross(front, m_Camera.up)) * m_Speed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (m_IsFlying)
			newPosition += m_Speed * m_Camera.up;
		else if (IsStandingOnGround())
			m_VelocityY = 7.5f;
	}
	if (m_IsFlying && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		newPosition -= m_Speed * m_Camera.up;

	Move(newPosition);

	m_Camera.position = m_Position;
}

void Player::Move(const glm::vec3& newPosition) noexcept
{
	if (newPosition == m_Position)
		return;

	if (!PLAYER_AABB.IntersectsBlocks(m_World, { newPosition.x, m_Position.y, m_Position.z }))
		m_Position.x = newPosition.x;

	if (!PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, m_Position.y, newPosition.z }))
		m_Position.z = newPosition.z;

	if (!PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, newPosition.y, m_Position.z }))
		m_Position.y = newPosition.y;
	else
	{
		if (newPosition.y < m_Position.y)
			m_Position.y = floor(m_Position.y) + 0.5f;
		else
			m_VelocityY = 0.0f;
	}
}

void Player::Update(double deltaTime) noexcept
{
	if (!m_IsFlying)
	{
		std::cout << IsStandingOnGround() << std::endl;

		if (m_VelocityY < 0.0f && IsStandingOnGround())
			m_VelocityY = 0.0f;

		m_VelocityY += GRAVITY * deltaTime;

		Move(glm::vec3 { m_Position.x, m_Position.y + (m_VelocityY * deltaTime), m_Position.z });
	}

	m_Camera.position = m_Position;
}

void Player::Render() const noexcept
{
}