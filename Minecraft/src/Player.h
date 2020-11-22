#pragma once

#include "Mob.h"
#include "World.h"
#include "Camera.h"
#include "AABB.h"

class Player : public Mob
{
public:
	inline static const AABB PLAYER_AABB = AABB({ -0.25f, -1.5f, -0.25f }, { 0.25f, 0.25f, 0.25f });

	Camera m_Camera;

	bool m_IsFlying = true;
	BlockType m_BlockInHand = static_cast<BlockType>(1);

	Player(World* world);
	
	void Input(GLFWwindow* window, double deltaTime);
	void Update(double deltaTime);
	void Render();

	std::optional<glm::vec3> GetTargetBlockPosition(int max, bool place = false) const;
};