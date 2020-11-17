#pragma once

#include "Mob.h"
#include "Mesh.h"

class Cow : public Mob
{
public:
	Cow(World* world, glm::vec3 position);
	
	void Update(double deltaTime);
	void Render();
};