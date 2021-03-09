#pragma once

#include "../World/World.h"
#include "../World/Block/Block.h"

class AABB
{
private:
	glm::vec3 m_Min, m_Max;

public:
	AABB(const glm::vec3& min, const glm::vec3& max) noexcept;
	
	bool IsPointInsideAABB(const glm::vec3& position, const glm::vec3& point) const noexcept;
	bool Intersects(const AABB& aabb, const glm::vec3& positionOne, const glm::vec3& positionTwo) const noexcept;
	bool IntersectsBlocks(const World* world, const glm::vec3& position) const noexcept;

	glm::vec3 GetMinimum() const noexcept { return m_Min; }
	glm::vec3 GetMaximum() const noexcept { return m_Max; }
	glm::vec3 GetRelativeMinimum(const glm::vec3& position) const noexcept { return m_Min + position; }
	glm::vec3 GetRelativeMaximum(const glm::vec3& position) const noexcept { return m_Max + position; }

	glm::vec3 getVN(const glm::vec3& normal, const glm::vec3& position) const noexcept;
	glm::vec3 getVP(const glm::vec3& normal, const glm::vec3& position) const noexcept;
};