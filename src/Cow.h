#pragma once

#include "Mob.h"
#include "Mesh.h"

class Cow : public Mob
{
public:
	Cow(World* world, const glm::vec3& position) noexcept;
	
	virtual void Update(double deltaTime) noexcept override;
	virtual void Render() const override;
};