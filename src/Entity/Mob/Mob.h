#pragma once

#include "../Entity.h"

class Mob : public Entity
{
public:
	unsigned int m_Health, m_MaxHealth;
	float m_Speed;

	Mob(World& world, const WorldPosition& position, unsigned int health, unsigned int maxHealth, float speed);
	
	virtual void Move(const WorldPosition& newPosition) = 0;

	virtual void Update(double deltaTime) = 0;
	virtual void Render() const = 0;
};
