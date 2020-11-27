#pragma once

#include "Mob.h"
#include "Mesh.h"

class Cow : public Mob
{
public:
	Cow(World* world, const glm::vec3& position) noexcept;
	
	void Update(double deltaTime) noexcept;
	void Render();
};