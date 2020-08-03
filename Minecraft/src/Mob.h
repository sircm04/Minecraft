#pragma once

#include "Entity.h"

class Mob : public Entity
{
public:
	int m_Health, m_MaxHealth;

	Mob(glm::vec3 position, int health, int maxHealth);
	
	virtual void Update(double deltaTime) = 0;
	virtual void Render(Shader* shader) = 0;
};