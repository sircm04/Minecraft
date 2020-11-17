#pragma once

#include "Entity.h"

class Mob : public Entity
{
public:
	int m_Health, m_MaxHealth;
	float m_Speed;

	Mob(World* world, glm::vec3 position = glm::vec3(), int health = 10, int maxHealth = 10, float speed = 9.0f);
	
	virtual void Update(double deltaTime) = 0;
	virtual void Render() = 0;
};