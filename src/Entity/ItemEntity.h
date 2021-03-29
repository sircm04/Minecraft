#pragma once

#include "../World/World.h"
#include "../Renderer/Shader.h"
#include "../Item.h"

class World;

class ItemEntity : public Entity
{
private:
	Item m_Item;

public:
	ItemEntity(Item item, World* world, const glm::vec3& position) noexcept;

	virtual void Update(double deltaTime) noexcept override;
	virtual void Render() const override;
};
