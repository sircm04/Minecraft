#include "../pch.h"
#include "AABB.h"

bool AABB::Intersects(const WorldPosition& position, const WorldPosition& point) const
{
	const WorldPosition oneMin = GetRelativeMinimum(position),
		oneMax = GetRelativeMaximum(position);

	return (point.x >= oneMin.x && point.x <= oneMax.x)
		&& (point.y >= oneMin.y && point.y <= oneMax.y)
		&& (point.z >= oneMin.z && point.z <= oneMax.z);
}

bool AABB::Intersects(const AABB& aabb, const WorldPosition& positionOne, const WorldPosition& positionTwo) const
{
	const WorldPosition oneMin = GetRelativeMinimum(positionOne),
		oneMax = GetRelativeMaximum(positionOne),
		twoMin = aabb.GetRelativeMinimum(positionTwo),
		twoMax = aabb.GetRelativeMaximum(positionTwo);

	return (oneMin.x <= twoMax.x && oneMax.x >= twoMin.x)
		&& (oneMin.y <= twoMax.y && oneMax.y >= twoMin.y)
		&& (oneMin.z <= twoMax.z && oneMax.z >= twoMin.z);
}

bool AABB::IntersectsBlocks(const World& world, const WorldPosition& position) const
{
	const glm::ivec3 blockMin = floor(GetRelativeMinimum(position)),
		blockMax = floor(GetRelativeMaximum(position));

	for (int x = blockMin.x; x <= blockMax.x; ++x) {
		for (int y = blockMin.y; y <= blockMax.y; ++y) {
			for (int z = blockMin.z; z <= blockMax.z; ++z) {
				const Block* block = world.GetBlock({ x, y, z });

				if (block && block->GetBlockTypeData().isSolid)
					return true;
			}
		}
	}

	return false;
}
