#pragma once

#include "../World/World.h"

#include "../Renderer/Shader.h"

class World;

class Entity
{
public:
	World* m_World;
	glm::vec3 m_Position;

	Entity(World* world, const glm::vec3& position = glm::vec3());
	virtual ~Entity() = default;

	virtual void Update(double deltaTime) = 0;
	virtual void Render() const = 0;
};