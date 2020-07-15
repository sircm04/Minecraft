#pragma once

#include "Shader.h"

class Entity
{
public:
	glm::vec3 m_Position;

	Entity(glm::vec3 position);
	virtual ~Entity() = default;

	virtual void Update(float deltaTime) = 0;
	virtual void Render(Shader* shader) = 0;
};