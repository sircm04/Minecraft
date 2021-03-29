#pragma once

#include "Mob.h"
#include "../../World/World.h"
#include "../../Camera.h"
#include "../../Physics/AABB.h"
#include "../../Inventory.h"

class Player : public Mob
{
public:
	static const inline AABB PLAYER_AABB = AABB({ -0.25f, -1.5f, -0.25f }, { 0.25f, 0.25f, 0.25f });

	static constexpr inline float GRAVITY = -24.0f;

	Camera m_Camera;

	Inventory m_Inventory;

	bool m_IsFlying = false;
	float m_VelocityY = 0;
	BlockType m_BlockInHand = static_cast<BlockType>(1);

	Player(World* world) noexcept;

	void Input(GLFWwindow* window, double deltaTime);

	virtual void Move(const glm::vec3& newPosition) noexcept override;
	
	virtual void Update(double deltaTime) noexcept override;
	virtual void Render() const noexcept override;

	const std::optional<glm::vec3> GetTargetBlockPosition(int max, bool place = false) const noexcept
	{
		return m_World->GetTargetBlockPosition(m_Position, m_Camera.front, max, place);
	}

	const bool IsStandingOnGround() const noexcept
	{
		return m_World->IsChunkLoaded(World::GetChunkPositionFromBlock({ m_Position.x, m_Position.z }))
			&& PLAYER_AABB.IntersectsBlocks(m_World, { m_Position.x, m_Position.y - 0.01f, m_Position.z });
	}
};
