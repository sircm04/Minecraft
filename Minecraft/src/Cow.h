#pragma once

#include "Mob.h"
#include "Mesh.h"

class Cow : public Mob
{
public:
	Cow(glm::vec3 position);
	
	void Update(double deltaTime);
	void Render(Shader* shader);
};