#include "../../pch.h"
#include "Mob.h"

Mob::Mob(World& world, const WorldPosition& position, unsigned int health, unsigned int maxHealth, float speed)
	: Entity(world, position), m_Health(health), m_MaxHealth(maxHealth), m_Speed(speed)
{
}
