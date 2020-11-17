#include "pch.h"
#include "Mob.h"

Mob::Mob(World* world, glm::vec3 position, int health, int maxHealth, float speed)
	: Entity(world, position), m_Health(health), m_MaxHealth(maxHealth), m_Speed(speed)
{
}