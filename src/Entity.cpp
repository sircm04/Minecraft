#include "pch.h"
#include "Entity.h"

Entity::Entity(World* world, const glm::vec3& position)
	: m_World(world), m_Position(position)
{
}