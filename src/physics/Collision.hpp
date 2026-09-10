#pragma once

#include <cstdint>
#include <span>

#include "physics/AABB.hpp"

// CollisionStats -- records results of a brute-force all-pairs collision query.
//
// candidateChecks: total number of unique (i, j) pairs examined where i < j.
//                  For N objects this always equals N*(N-1)/2.
//
// intersections:   number of pairs where the two AABBs actually overlap.
struct CollisionStats {
    std::uint64_t candidateChecks{0};
    std::uint64_t intersections{0};
};

// bruteForceAllPairs -- O(N^2) brute-force AABB collision detection.
//
// Tests every unique unordered pair (i, j) where 0 <= i < j < N.
// This produces exactly N*(N-1)/2 candidate checks -- no duplicates,
// no self-tests.
//
// This function exists specifically as a measurable BASELINE for a future
// spatial-grid broad-phase optimization. It is NOT the game's final
// collision architecture. A single AABB::intersects() call is O(1); the
// nested loop that calls it N*(N-1)/2 times is O(N^2).
//
// Parameters:
//   boxes -- read-only span of AABBs to test against each other.
//
// Returns a CollisionStats struct populated with counts.
CollisionStats bruteForceAllPairs(std::span<const AABB> boxes);
