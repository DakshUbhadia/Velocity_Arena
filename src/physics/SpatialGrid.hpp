#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "physics/AABB.hpp"

// GridCellCoord -- integer (x, z) index for a single XZ grid cell.
// operator== is defaulted (C++20) for use in unordered containers.
struct GridCellCoord {
    int x{0};
    int z{0};
    bool operator==(const GridCellCoord&) const = default;
};

// GridCellCoordHash -- hash using prime-multiplier mixing.
struct GridCellCoordHash {
    std::size_t operator()(const GridCellCoord& c) const noexcept
    {
        std::size_t h = static_cast<std::size_t>(c.x) * 2654435761ULL;
        h ^= static_cast<std::size_t>(c.z) * 2246822519ULL;
        return h;
    }
};

// ObjectPair -- canonicalised unordered pair of object indices.
// Invariant: first <= second (caller uses std::min/std::max).
// Ensures (i,j) and (j,i) hash to the same key.
struct ObjectPair {
    std::size_t first{};
    std::size_t second{};
    bool operator==(const ObjectPair&) const = default;
};

// ObjectPairHash -- hash for ObjectPair.
struct ObjectPairHash {
    std::size_t operator()(const ObjectPair& p) const noexcept
    {
        std::size_t h = p.first * 2654435761ULL;
        h ^= p.second * 2246822519ULL;
        return h;
    }
};

// SpatialGridStats -- diagnostics from spatialGridAllPairs().
//
// candidateChecks       -- unique (i,j) pairs sent to AABB::intersects().
//                          Duplicate pairs from multi-cell overlap excluded.
// intersections         -- pairs that truly overlap.
// rawCellPairVisits     -- encounters before deduplication.
//                          rawCellPairVisits == candidateChecks + duplicatePairsSkipped.
// duplicatePairsSkipped -- pairs suppressed because seen in a prior cell.
// cellInsertions        -- total object-to-cell assignments.
// occupiedCells         -- distinct non-empty cells after all insertions.
struct SpatialGridStats {
    std::uint64_t candidateChecks{0};
    std::uint64_t intersections{0};
    std::uint64_t rawCellPairVisits{0};
    std::uint64_t duplicatePairsSkipped{0};
    std::uint64_t cellInsertions{0};
    std::size_t   occupiedCells{0};
};

// SpatialGrid -- uniform XZ spatial hash grid for broad-phase collision.
//
// Y is NOT partitioned; gameplay is on the XZ ground plane and all
// benchmark AABBs share the same Y range.
//
// Cell coordinate mapping:
//   cellX = floor(worldX / cellSize)
//   cellZ = floor(worldZ / cellSize)
//
// std::floor is mandatory -- integer truncation is incorrect for negatives:
//   worldX = -0.5, cellSize = 2.0:
//     floor(-0.25) = -1   <-- correct negative cell
//     (int)(-0.25) =  0   <-- WRONG (truncates toward zero)
//
// Each AABB is inserted into ALL cells it overlaps, not just its centre.
// This preserves correctness when a collision straddles a cell boundary.
class SpatialGrid {
public:
    // Construct with the given cell side length.
    // Throws std::invalid_argument if cellSize <= 0.
    explicit SpatialGrid(float cellSize);

    // Remove all stored indices and clear the cell map.
    // Cell size is preserved for reuse.
    void clear();

    // Insert objectIndex into every XZ cell overlapped by bounds.
    void insert(std::size_t objectIndex, const AABB& bounds);

    float       cellSize()          const noexcept;
    std::size_t occupiedCellCount() const noexcept;

    // Read-only access to the internal cell map.
    const std::unordered_map<GridCellCoord,
                             std::vector<std::size_t>,
                             GridCellCoordHash>& cells() const noexcept;

private:
    // Convert a world-space coordinate to an integer cell index.
    // Uses std::floor to handle negative values correctly.
    int worldToCell(float coordinate) const noexcept;

    float cellSize_;
    std::unordered_map<GridCellCoord,
                       std::vector<std::size_t>,
                       GridCellCoordHash> cells_;
};
