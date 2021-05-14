#pragma once

#include "../Positions.h"

class AABB;

struct Plane
{
    float DistanceToPoint(const WorldPosition& point) const;

    float distanceToOrigin;
    glm::vec3 normal;
};

class ViewFrustum
{
public:
    void Update(const glm::mat4& projViewMatrix);

    bool IsAABBInFrustum(const AABB& aabb, const WorldPosition& position) const;

private:
    std::array<Plane, 6> m_Planes;
};
