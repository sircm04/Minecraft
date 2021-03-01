#include "../../pch.h"
#include "Cow.h"

#include "../../Assets.h"

Cow::Cow(World* world, const glm::vec3& position) noexcept
	: Mob(world, position)
{
}

void Cow::Move(const glm::vec3& newPosition) noexcept
{
}

void Cow::Update(double deltaTime) noexcept
{
}

void Cow::Render() const
{
	Assets::SHADERS["ENTITY"]->Bind();
	Assets::SHADERS["ENTITY"]->SetMat4("model", glm::translate(glm::mat4(1.0f), m_Position));
	Assets::TEXTURES["COW"]->Bind();
	Assets::MESHES["COW"]->Bind();
	Assets::MESHES["COW"]->Render();
}