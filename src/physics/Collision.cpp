#include "physics/Collision.hpp"

#include <unordered_set>

// bruteForceAllPairs -- O(N^2) brute-force AABB collision detection. ─────────
//
// N*(N-1)/2 unique unordered pairs.
// Starting j at i+1 avoids:
//   (i, i) -- self-collision
//   (j, i) -- duplicate of (i, j)
//
// candidateChecks will always equal N*(N-1)/2 after this loop.
// This is used during benchmarking to verify correctness of the loop.
//
// This function is the REFERENCE implementation and must NOT be removed.
// It provides the correctness baseline against which the spatial grid is
// verified.

CollisionStats bruteForceAllPairs(std::span<const AABB> boxes)
{
    CollisionStats stats{};

    const std::size_t N = boxes.size();

    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = i + 1; j < N; ++j) {
            ++stats.candidateChecks;

            if (boxes[i].intersects(boxes[j])) {
                ++stats.intersections;
            }
        }
    }

    return stats;
}

// spatialGridAllPairs -- optimised broad-phase using a uniform XZ grid. ───────
//
// The spatial grid reduces how many unique (i,j) pairs reach AABB::intersects()
// by only generating pairs for objects that share at least one grid cell.
//
// The narrow-phase test (AABB::intersects) is the SAME O(1) function as
// brute force. The grid only reduces the number of times it is invoked.
//
// Timing for benchmarking MUST cover this entire function including:
//   - grid construction
//   - AABB insertion (potentially multi-cell)
//   - candidate enumeration
//   - duplicate pair suppression
//   - AABB intersection testing
//
// Complexity:
//   Grid construction: O(N + cellInsertions)
//   Candidate generation: O(sum of C(k,2) for each cell with k objects)
//   Expected O(N + K) for uniform distributions, where K = nearby candidate pairs.
//   Worst case O(N^2) when all objects land in one cell.
//   Note: unordered_map/set operations are average O(1), not guaranteed worst-case O(1).

SpatialGridStats spatialGridAllPairs(std::span<const AABB> boxes,
                                     float cellSize)
{
    SpatialGridStats stats{};

    const std::size_t N = boxes.size();
    if (N == 0) return stats;

    // ── 1. Build the grid: insert every AABB into all overlapping cells ──────
    SpatialGrid grid(cellSize);

    for (std::size_t i = 0; i < N; ++i) {
        
        // We need to count insertions: track size before/after is less
        // efficient; instead count via the grid's cells.
        // We'll count insertions below using a separate pass-through approach.
        grid.insert(i, boxes[i]);
    }

    // Recount cellInsertions by summing cell sizes.
    for (const auto& [coord, indices] : grid.cells()) {
        stats.cellInsertions += static_cast<std::uint64_t>(indices.size());
    }
    stats.occupiedCells = grid.occupiedCellCount();

    // ── 2. Generate candidate pairs and deduplicate ──────────────────────────
    std::unordered_set<ObjectPair, ObjectPairHash> seenPairs;
    // Reserve capacity proportional to the number of objects to reduce
    // rehashing. The actual number of unique pairs is unknown a priori.
    seenPairs.reserve(N * 4);

    for (const auto& [coord, indices] : grid.cells()) {
        const std::size_t cellN = indices.size();

        // For each unique local pair in this cell:
        for (std::size_t li = 0; li < cellN; ++li) {
            for (std::size_t lj = li + 1; lj < cellN; ++lj) {
                ++stats.rawCellPairVisits;

                // Canonicalise: first = min(a,b), second = max(a,b).
                // This ensures (i,j) and (j,i) are the same key.
                const std::size_t a = indices[li];
                const std::size_t b = indices[lj];
                const ObjectPair pair{
                    a < b ? a : b,
                    a < b ? b : a
                };

                // Skip if already processed from another shared cell.
                auto [it, inserted] = seenPairs.insert(pair);
                if (!inserted) {
                    ++stats.duplicatePairsSkipped;
                    continue;
                }

                // Unique pair: run narrow-phase test.
                ++stats.candidateChecks;

                if (boxes[pair.first].intersects(boxes[pair.second])) {
                    ++stats.intersections;
                }
            }
        }
    }

    return stats;
}
