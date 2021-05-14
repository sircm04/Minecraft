#pragma once

#include "../World/World.h"
#include "../World/Block/Block.h"

class AABB
{
private:
	WorldPosition m_Min, m_Max;

public:
	constexpr AABB::AABB(const WorldPosition& min, const WorldPosition& max)
		: m_Min(min), m_Max(max)
	{
	}
	
	bool Intersects(const WorldPosition& position, const WorldPosition& point) const;
	bool Intersects(const AABB& aabb, const WorldPosition& positionOne, const WorldPosition& positionTwo) const;
	bool IntersectsBlocks(const World& world, const WorldPosition& position) const;

	WorldPosition GetMinimum() const { return m_Min; }
	WorldPosition GetMaximum() const { return m_Max; }
	WorldPosition GetRelativeMinimum(const WorldPosition& position) const { return m_Min + position; }
	WorldPosition GetRelativeMaximum(const WorldPosition& position) const { return m_Max + position; }
	WorldPosition getVN(const WorldPosition& normal, const WorldPosition& position) const;
	WorldPosition getVP(const WorldPosition& normal, const WorldPosition& position) const;
};
