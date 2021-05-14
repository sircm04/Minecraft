#include "../pch.h"
#include "Entity.h"

Entity::Entity(World& world, const WorldPosition& position)
	: m_World(world), m_Position(position)
{
}
