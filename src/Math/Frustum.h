#pragma once

#include <array>

class AABB;

struct Plane
{
    float DistanceToPoint(const glm::vec3& point) const;

    float distanceToOrigin;
    glm::vec3 normal;
};

class ViewFrustum
{
public:
    void Update(const glm::mat4& projViewMatrix) noexcept;

    bool IsBoxInFrustum(const AABB& box, const glm::vec3& position) const noexcept;

private:
    std::array<Plane, 6> m_Planes;
};
