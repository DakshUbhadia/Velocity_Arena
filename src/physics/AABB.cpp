#include "physics/AABB.hpp"

#include <cassert>
#include <algorithm>

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
    : min_(min), max_(max)
{
    // In debug builds, verify that min <= max on every axis.
    assert(min_.x <= max_.x && "AABB: min.x > max.x");
    assert(min_.y <= max_.y && "AABB: min.y > max.y");
    assert(min_.z <= max_.z && "AABB: min.z > max.z");
}

AABB AABB::fromCenterHalfExtents(const glm::vec3& center,
                                  const glm::vec3& halfExtents)
{
    // Half extents must be non-negative.
    // A negative half-extent would produce an inverted (inside-out) AABB.
    assert(halfExtents.x >= 0.0f && "AABB: halfExtents.x is negative");
    assert(halfExtents.y >= 0.0f && "AABB: halfExtents.y is negative");
    assert(halfExtents.z >= 0.0f && "AABB: halfExtents.z is negative");

    return AABB(center - halfExtents, center + halfExtents);
}

bool AABB::intersects(const AABB& other) const
{
    // Separating Axis Theorem for AABBs:
    // Two AABBs are SEPARATED if there exists any axis where their
    // intervals do not overlap. We test X, Y, Z in order and
    // return false (no intersection) as soon as we find a gap.
    // If all three axes overlap, the boxes intersect.
    //
    // Touching boundaries (max of one == min of other) count as
    // intersection (using >= and <= rather than > and <).

    if (max_.x < other.min_.x || min_.x > other.max_.x) return false;
    if (max_.y < other.min_.y || min_.y > other.max_.y) return false;
    if (max_.z < other.min_.z || min_.z > other.max_.z) return false;

    return true;
}

const glm::vec3& AABB::min() const { return min_; }
const glm::vec3& AABB::max() const { return max_; }
