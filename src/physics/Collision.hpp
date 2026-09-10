#pragma once

#include <cstdint>
#include <span>

#include "physics/AABB.hpp"
#include "physics/SpatialGrid.hpp"

// CollisionStats -- results of a brute-force all-pairs collision query.
//
// candidateChecks: total unique (i, j) pairs examined where i < j.
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
// This is the REFERENCE implementation for correctness comparison.
// It remains permanently available as the baseline.
//
// Parameters:
//   boxes -- read-only span of AABBs to test against each other.
//
// Returns a CollisionStats struct populated with counts.
CollisionStats bruteForceAllPairs(std::span<const AABB> boxes);

// spatialGridAllPairs -- optimised broad-phase using a uniform XZ spatial grid.
//
// Algorithm:
//   1. Insert each AABB into every XZ cell it overlaps.
//   2. For every occupied cell, generate candidate pairs from cell contents.
//   3. Deduplicate pairs that appear in multiple cells.
//   4. Call AABB::intersects() only on unique candidate pairs.
//
// The spatial grid reduces candidateChecks compared to bruteForceAllPairs
// for sparse/uniform workloads. The narrow-phase test (AABB::intersects)
// is the SAME O(1) function used by brute force -- the grid only reduces
// how many times it is called.
//
// Parameters:
//   boxes    -- read-only span of AABBs (must be the SAME vector as brute force).
//   cellSize -- XZ cell side length in world units. Must be > 0.
//               Recommended starting value: 2.0 for 1x1x1 AABBs.
//
// Returns a SpatialGridStats struct populated with full diagnostics.
SpatialGridStats spatialGridAllPairs(std::span<const AABB> boxes,
                                     float cellSize);
