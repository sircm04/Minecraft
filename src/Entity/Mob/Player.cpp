#include "../../pch.h"
#include "Player.h"

Player::Player(World& world)
	: Mob(world, WorldPosition(), 10, 10, 15.0f)
{
}

void Player::Input(GLFWwindow* window, double deltaTime)
{
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
