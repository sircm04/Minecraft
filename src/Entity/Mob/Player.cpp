#include "../../pch.h"
#include "Player.h"

#include "../../Game.h"
#include "../../Utils/Utils.h"

Player::Player(World& world, const WorldPosition& position)
	: Mob(world, position, 10, 10, 7.0f), m_NumSteps(0), m_BlockInHand(static_cast<BlockType>(1)), m_Velocity({ 0.0f, 0.0f, 0.0f })
{
}

void Player::Input(GLFWwindow* window, double deltaTime)
{
	static float clickDelay[3];
	static constexpr uint8_t maxBlockTypeKeys = std::min(static_cast<uint8_t>(BlockType::Count), static_cast<uint8_t>(10));

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && clickDelay[0] == 0)
	{
		auto blockPosition = GetTargetBlockPosition(10, false);

		if (blockPosition)
		{
			const ChunkLocation chunkLocation = PosUtils::ConvertWorldPosToChunkLoc(*blockPosition);
			Chunk* chunk = m_World.GetChunk(chunkLocation);
			chunk->SetBlock(PosUtils::ConvertWorldPosToChunkPos(*blockPosition), { BlockType::Air });
			chunk->GenerateMesh(m_World, chunkLocation);
			auto neighbors = m_World.GetNeighboringChunks(*blockPosition);
			for (auto& element : neighbors)
				element.second->GenerateMesh(m_World, element.first);
			clickDelay[0] = 0.2f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && clickDelay[0] == 0)
	{
		auto blockPosition = GetTargetBlockPosition(10, true);

		if (blockPosition && !PLAYER_AABB.Intersects({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }, m_Position, *blockPosition))
		{
			const ChunkLocation chunkLocation = PosUtils::ConvertWorldPosToChunkLoc(*blockPosition);
			Chunk* chunk = m_World.GetChunk(chunkLocation);
			chunk->SetBlock(PosUtils::ConvertWorldPosToChunkPos(*blockPosition), { m_BlockInHand });
			chunk->GenerateMesh(m_World, chunkLocation);
			auto neighbors = m_World.GetNeighboringChunks(*blockPosition);
			for (auto& element : neighbors)
				element.second->GenerateMesh(m_World, element.first);
			clickDelay[0] = 0.2f;
		}
	}

	for (uint8_t i = 1; i < maxBlockTypeKeys; ++i)
		if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS)
			m_BlockInHand = static_cast<BlockType>(i);

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && clickDelay[1] == 0)
	{
		m_IsFlying = !m_IsFlying;
		if (m_IsFlying)
			m_Velocity.y = 0.0f;
		clickDelay[1] = 0.2f;
	}

    WorldPosition front = glm::normalize(glm::vec3 { m_Camera.front.x, 0.0f, m_Camera.front.z }),
        position = m_Position;

    float speed = m_Speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		speed = m_Speed * deltaTime * 1.5f;

	bool moved = false, onGround = IsStandingOnGround();

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
        position -= glm::normalize(glm::cross(front, m_Camera.up)) * speed;
		m_WalkingDirection = Direction::West;
		moved = true;
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
        position += glm::normalize(glm::cross(front, m_Camera.up)) * speed;
		m_WalkingDirection = Direction::East;
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += speed * front;
		m_WalkingDirection = Direction::North;
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position -= speed * front;
		m_WalkingDirection = Direction::South;
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (m_IsFlying)
			position += speed * m_Camera.up;
		else if (onGround)
			m_Velocity.y = 7.5f;
	}
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && m_IsFlying)
        position -= speed * m_Camera.up;

	if (moved)
	{
		if (!m_IsFlying && onGround)
			m_NumSteps += speed * 0.75f;
	}
	else
	{
		float zeroPoint = round(m_NumSteps / 3.14) * 3.14;
		m_NumSteps = Utils::ApproachValue<float>(m_NumSteps, zeroPoint, speed * 0.75f);
	}

    Move(position);

	for (auto& delay : clickDelay) {
		if (delay > 0)
			delay -= 1 * deltaTime;
		if (delay < 0)
			delay = 0;
	}

	m_NumSteps = fmod(m_NumSteps, 3.14 * 2);
}

void Player::Move(const WorldPosition& position)
{
    if (position == m_Position)
        return;

    if (!PLAYER_AABB.IntersectsBlocks(m_World, { position.x, m_Position.y, m_Position.z }))
        m_Position.x = position.x;

    if (!PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, m_Position.y, position.z }))
        m_Position.z = position.z;

	if (!PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, position.y, m_Position.z }))
	{
		m_Position.y = position.y;
	}
	else
	{
		if (position.y < m_Position.y)
			m_Position.y = floor(m_Position.y) + 0.5f;
		else
			m_Velocity.y = 0.0f;
	}
}

void Player::Update(double deltaTime)
{
	if (!IsStandingOnGround())
	{
		if (!m_IsFlying)
			m_Velocity.y -= Game::GRAVITY * deltaTime;
	}
	else if (m_Velocity.y < 0.0f)
	{
		m_Velocity.y = 0.0f;
	}

	glm::vec3 velocity = {
		m_Velocity.x * deltaTime,
		m_Velocity.y * deltaTime,
		m_Velocity.z * deltaTime
	};

	Move(m_Position + velocity);
}

void Player::Render() const
{
}

const std::optional<WorldPosition> Player::GetTargetBlockPosition(int max, bool place) const
{
	WorldPosition position = m_Position, blockPos;
	glm::vec3 sign;

	for (uint8_t i = 0; i < 3; ++i)
		sign[i] = m_Camera.front[i] > 0;

	for (int i = 0; i < max; ++i) {
		glm::vec3 tvec = (floor(position + sign) - position) / m_Camera.front;
		float t = std::min(tvec.x, std::min(tvec.y, tvec.z));

		position += m_Camera.front * (t + 0.001f);

		const Block* block = m_World.GetBlock(blockPos = floor(position));

		if (block && block->GetBlockTypeData().isSolid)
		{
			if (place)
			{
				glm::vec3 normal;
				for (uint8_t i = 0; i < 3; ++i) {
					normal[i] = (t == tvec[i]);
					if (sign[i]) normal[i] = -normal[i];
				}

				position += normal;

				blockPos = floor(position);
			}

			return blockPos;
		}
	}

	return std::nullopt;
}

bool Player::IsStandingOnGround() const
{
	const Chunk* chunk = m_World.GetChunk(PosUtils::ConvertWorldPosToChunkLoc(m_Position));

	return chunk && chunk->m_ChunkState >= ChunkState::Generated
		&& PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, m_Position.y - 0.01f, m_Position.z });
}
