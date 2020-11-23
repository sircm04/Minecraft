#include "pch.h"
#include "AABB.h"

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
	: m_Min(min), m_Max(max)
{
}

bool AABB::IsPointInsideAABB(const glm::vec3& position, const glm::vec3& point) const
{
	glm::vec3 oneMin = GetRelativeMinimum(position),
		oneMax = GetRelativeMaximum(position);

	return (point.x >= oneMin.x && point.x <= oneMax.x)
		&& (point.y >= oneMin.y && point.y <= oneMax.y)
		&& (point.z >= oneMin.z && point.z <= oneMax.z);
}

bool AABB::Intersects(const AABB& aabb, const glm::vec3& positionOne, const glm::vec3& positionTwo) const
{
	glm::vec3 oneMin = GetRelativeMinimum(positionOne),
		oneMax = GetRelativeMaximum(positionOne),
		twoMin = aabb.GetRelativeMinimum(positionTwo),
		twoMax = aabb.GetRelativeMaximum(positionTwo);

	return (oneMin.x <= twoMax.x && oneMax.x >= twoMin.x)
		&& (oneMin.y <= twoMax.y && oneMax.y >= twoMin.y)
		&& (oneMin.z <= twoMax.z && oneMax.z >= twoMin.z);
}

bool AABB::IntersectsBlocks(const World* world, const glm::vec3& position) const
{
	glm::ivec3 blockMin = floor(GetRelativeMinimum(position)),
		blockMax = floor(GetRelativeMaximum(position));

	for (int x = blockMin.x; x <= blockMax.x; ++x) {
		for (int y = blockMin.y; y <= blockMax.y; ++y) {
			for (int z = blockMin.z; z <= blockMax.z; ++z) {
				const Block* block = world->GetBlock({ x, y, z });

				if (block && block->GetBlockTypeData().isSolid)
					return true;
			}
		}
	}

	return false;
}

glm::vec3 AABB::getVN(const glm::vec3& normal, const glm::vec3& position) const
{
	glm::vec3 res = GetRelativeMinimum(position);

    if (normal.x < 0)
        res.x += m_Max.x;
    if (normal.y < 0)
        res.y += m_Max.y;
    if (normal.z < 0)
        res.z += m_Max.z;

    return res;
}

glm::vec3 AABB::getVP(const glm::vec3& normal, const glm::vec3& position) const
{
	glm::vec3 res = GetRelativeMinimum(position);

    if (normal.x > 0)
        res.x += m_Max.x;
    if (normal.y > 0)
        res.y += m_Max.y;
    if (normal.z > 0)
        res.z += m_Max.z;

    return res;
}