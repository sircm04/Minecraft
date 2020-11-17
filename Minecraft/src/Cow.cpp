#include "pch.h"
#include "Cow.h"

#include "Assets.h"

Cow::Cow(World* world, glm::vec3 position)
	: Mob(world, position)
{
}

void Cow::Update(double deltaTime)
{
}

void Cow::Render()
{
	Assets::ENTITY_SHADER->SetMat4("model", glm::translate(glm::mat4(1.0f), m_Position));
	Assets::COW_TEXTURE->Bind();
	Assets::COW_MESH->Bind();
	Assets::COW_MESH->Render();
}