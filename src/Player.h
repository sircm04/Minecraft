#pragma once

#include "Mob.h"
#include "World.h"
#include "Camera.h"
#include "AABB.h"

class Player : public Mob
{
public:
	static const inline AABB PLAYER_AABB = AABB({ -0.25f, -1.5f, -0.25f }, { 0.25f, 0.25f, 0.25f });

	Camera m_Camera;

	bool m_IsFlying = true, m_IsFalling = false;
	float m_VelocityY, m_AccelerationY = 0;
	BlockType m_BlockInHand = static_cast<BlockType>(1);

	Player(World* world) noexcept;
	
	void Input(GLFWwindow* window, double deltaTime);
	virtual void Update(double deltaTime) noexcept override;
	virtual void Render() const noexcept override;

	const inline std::optional<glm::vec3> GetTargetBlockPosition(int max, bool place = false) const noexcept;
};