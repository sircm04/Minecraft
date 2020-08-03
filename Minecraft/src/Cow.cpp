#include "pch.h"
#include "Cow.h"

#include "Assets.h"

Cow::Cow(glm::vec3 position)
	: Mob(position, 9, 9)
{
}

void Cow::Update(double deltaTime)
{
}

void Cow::Render(Shader* shader)
{
	shader->SetMat4("model", glm::translate(glm::mat4(1.0f), m_Position));
	Assets::COW_TEXTURE->Bind();
	Assets::COW_MESH->Bind();
	Assets::COW_MESH->Render();
}