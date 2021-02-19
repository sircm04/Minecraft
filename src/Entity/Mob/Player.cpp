#include "../../pch.h"
#include "Player.h"

Player::Player(World* world) noexcept
	: Mob(world, glm::vec3(), 10, 10, 9.0f)
{
}

void Player::Input(GLFWwindow* window, double deltaTime)
{
	static float clickDelay[2];
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

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && clickDelay[1] == 0)
	{
		m_IsFlying = !m_IsFlying;
		clickDelay[1] = 0.2f;
	}

	for (auto& delay : clickDelay) {
		if (delay > 0)
			delay -= 1 * deltaTime;
		if (delay < 0)
			delay = 0;
	}

	for (uint8_t i = 1; i < maxBlockTypeKeys; ++i)
	{
		if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS)
			m_BlockInHand = static_cast<BlockType>(i);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		m_Speed *= ((m_IsFlying) ? 5.0f : 1.5f);

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
	{
		if (m_IsFlying)
		{
			newPosition += m_Speed * m_Camera.up;
		}
		else if (IsStandingOnGround())
		{
			m_AccelerationY = 0.01f;
			m_VelocityY = 9.5f;
		}
	}
	if (m_IsFlying && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		newPosition -= m_Speed * m_Camera.up;

	Move(newPosition);

	m_Camera.position = m_Position;
}

void Player::Move(glm::vec3 newPosition) noexcept
{
	if (newPosition == m_Position)
		return;

	if (!PLAYER_AABB.IntersectsBlocks(m_World, { newPosition.x, m_Position.y, m_Position.z }))
		m_Position.x = newPosition.x;

	if (!PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, m_Position.y, newPosition.z }))
		m_Position.z = newPosition.z;

	if (!PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, newPosition.y, m_Position.z }))
		m_Position.y = newPosition.y;
}

void Player::Update(double deltaTime) noexcept
{
	if (!m_IsFlying)
	{
		m_VelocityY = std::clamp(m_VelocityY, -100.0f, 100.0f);

		if (IsStandingOnGround())
		{
			m_VelocityY = 0.0f;
			m_AccelerationY = 0.0f;
		}
		else
		{
			m_VelocityY += m_AccelerationY;

			if (m_AccelerationY > -0.01f)
				m_AccelerationY -= 0.005f;
		}

		std::cout << m_VelocityY << ", " << m_AccelerationY << "\n";

		Move({ m_Position.x, m_Position.y + (m_VelocityY * deltaTime), m_Position.z });
	}

	m_Camera.position = m_Position;
}

void Player::Render() const noexcept
{
}

const inline std::optional<glm::vec3> Player::GetTargetBlockPosition(int max, bool place) const noexcept
{
	return m_World->GetTargetBlockPosition(m_Position, m_Camera.front, max, place);
}

const inline bool Player::IsStandingOnGround() const noexcept
{
	const Block* block = m_World->GetBlock(glm::floor(glm::vec3{ m_Position.x, m_Position.y + PLAYER_AABB.GetMinimum().y - 0.01f, m_Position.z }));

	return (!block || block->GetBlockTypeData().isSolid);
}