#pragma once

#include "../World/World.h"
#include "../Renderer/Shader.h"

class World;

class Entity
{
public:
	World& m_World;
	WorldPosition m_Position;

	Entity(World& world, const WorldPosition& position);
	virtual ~Entity() = default;

	virtual void Update(double deltaTime) = 0;
	virtual void Render() const = 0;
};
