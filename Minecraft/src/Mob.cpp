#include "pch.h"
#include "Mob.h"

Mob::Mob(glm::vec3 position, int health, int maxHealth)
	: Entity(position), m_Health(health), m_MaxHealth(maxHealth)
{
}