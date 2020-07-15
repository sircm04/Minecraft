#pragma once

#include "Mob.h"
#include "Mesh.h"

class Cow : public Mob
{
public:
	Cow(glm::vec3 position);
	
	void Update(float deltaTime);
	void Render(Shader* shader);
};