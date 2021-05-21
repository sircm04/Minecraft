#pragma once

#include "Mob.h"
#include "../../Camera.h"
#include "../../Physics/AABB.h"

class Player : public Mob
{
private:
	static constexpr inline AABB PLAYER_AABB = AABB({ -0.25f, -1.5f, -0.25f }, { 0.25f, 0.25f, 0.25f });

public:
	Camera m_Camera;
	glm::vec3 m_Velocity;

	bool m_IsFlying = true;

	float m_NumSteps;

	BlockType m_BlockInHand;

	Player(World& world);

	void Input(GLFWwindow* window, double deltaTime);

	virtual void Move(const WorldPosition& position) override;
	
	virtual void Update(double deltaTime) override;
	virtual void Render() const override;

	const std::optional<WorldPosition> GetTargetBlockPosition(int max, bool place) const;
	bool Player::IsStandingOnGround() const;
};
