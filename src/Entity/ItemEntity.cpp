#include "../pch.h"
#include "ItemEntity.h"

ItemEntity::ItemEntity(Item item, World* world, const glm::vec3& position) noexcept
	: m_Item(item), Entity(world, position)
{
}

void ItemEntity::Update(double deltaTime) noexcept
{

}

void ItemEntity::Render() const
{
	
}