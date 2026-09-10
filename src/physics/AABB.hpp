#pragma once

#include <glm/vec3.hpp>

// AABB -- Axis-Aligned Bounding Box
//
// A simple 3D bounding volume defined by a minimum and maximum corner point.
// Both points are in world space. The box sides are always parallel to the
// world axes (hence "axis-aligned"), which makes intersection tests cheap.
//
// Construction options:
//   AABB(min, max)                   -- from explicit corner points
//   AABB::fromCenterHalfExtents(...) -- from center + half-extents (common game convention)
//
// Intersection semantics:
//   Touching faces, edges, and corners are considered INTERSECTING (>=/<= comparisons).
//   This is the standard convention for game collision.

class AABB {
public:
    // Construct from explicit min/max corner points.
    // Precondition: min <= max component-wise (asserted in debug builds).
    AABB(const glm::vec3& min, const glm::vec3& max);

    // Factory: build from a center point and half-extents.
    //   min = center - halfExtents
    //   max = center + halfExtents
    //
    // Precondition: all halfExtents components must be >= 0.
    // An assert fires in debug builds if any component is negative.
    static AABB fromCenterHalfExtents(const glm::vec3& center,
                                      const glm::vec3& halfExtents);

    // Returns true if this AABB overlaps with \'other\' on all three axes.
    //
    // Two AABBs intersect if and only if their intervals overlap on EVERY axis.
    // If there is a separating axis (no overlap on X, Y, or Z), they do not collide.
    //
    // Touching boundaries (e.g. one max == other min) count as intersecting.
    //
    // Implementation uses early-out per axis for efficiency.
    bool intersects(const AABB& other) const;

    const glm::vec3& min() const;
    const glm::vec3& max() const;

private:
    glm::vec3 min_;
    glm::vec3 max_;
};
