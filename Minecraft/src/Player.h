#pragma once

#include "Mob.h"
#include "World.h"
#include "Camera.h"
#include "AABB.h"

class Player : public Mob
{
private:
	World* m_World;

public:
	static const AABB PLAYER_AABB;

	Camera m_Camera;

	Player(World* world);
	
	void Input(GLFWwindow* window, float deltaTime);
	void Update(float deltaTime);
	void Render(Shader* shader);

	bool TestCollision(const glm::vec3& position);

	std::optional<glm::vec3> GetTargetBlockPosition(int max, bool place = false);
};