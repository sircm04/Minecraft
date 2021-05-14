#include "../../pch.h"
#include "Player.h"

Player::Player(World& world)
	: Mob(world, WorldPosition(), 10, 10, 15.0f)
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

	for (auto& delay : clickDelay) {
		if (delay > 0)
			delay -= 1 * deltaTime;
		if (delay < 0)
			delay = 0;
	}

	for (uint8_t i = 1; i < maxBlockTypeKeys; ++i)
		if (glfwGetKey(window, GLFW_KEY_0 + i) == GLFW_PRESS)
			m_BlockInHand = static_cast<BlockType>(i);

    WorldPosition front = glm::normalize(glm::vec3 { m_Camera.front.x, 0.0f, m_Camera.front.z }),
        position = m_Position;
    const float speed = m_Speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += speed * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= speed * front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(front, m_Camera.up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(front, m_Camera.up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += speed * m_Camera.up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= speed * m_Camera.up;

    Move(position);
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
        m_Position.y = position.y;
}

void Player::Update(double deltaTime)
{
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
