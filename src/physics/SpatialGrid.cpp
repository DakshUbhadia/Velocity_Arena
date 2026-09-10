#include "physics/SpatialGrid.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

// SpatialGrid -------------------------------------------------------------------

SpatialGrid::SpatialGrid(float cellSize)
    : cellSize_(cellSize)
{
    if (cellSize_ <= 0.0f) {
        throw std::invalid_argument(
            "SpatialGrid: cellSize must be positive, got " +
            std::to_string(cellSize_));
    }
}

void SpatialGrid::clear()
{
    cells_.clear();
}

void SpatialGrid::insert(std::size_t objectIndex, const AABB& bounds)
{
    // Determine which cells this AABB overlaps on the XZ plane.
    // Using floor() is essential for correct negative coordinate handling.
    //
    // Example with cellSize = 2.0:
    //   bounds.min().x = -0.5  ->  floor(-0.25) = -1  (cell -1)
    //   bounds.max().x =  1.8  ->  floor( 0.90) =  0  (cell  0)
    //   => insert into cells -1 and 0 on X.

    const int minCellX = worldToCell(bounds.min().x);
    const int maxCellX = worldToCell(bounds.max().x);
    const int minCellZ = worldToCell(bounds.min().z);
    const int maxCellZ = worldToCell(bounds.max().z);

    for (int cx = minCellX; cx <= maxCellX; ++cx) {
        for (int cz = minCellZ; cz <= maxCellZ; ++cz) {
            cells_[GridCellCoord{cx, cz}].push_back(objectIndex);
        }
    }
}

float SpatialGrid::cellSize() const noexcept
{
    return cellSize_;
}

std::size_t SpatialGrid::occupiedCellCount() const noexcept
{
    return cells_.size();
}

const std::unordered_map<GridCellCoord,
                         std::vector<std::size_t>,
                         GridCellCoordHash>&
SpatialGrid::cells() const noexcept
{
    return cells_;
}

int SpatialGrid::worldToCell(float coordinate) const noexcept
{
    // std::floor rounds toward negative infinity, which correctly handles
    // negative world coordinates. Simple integer cast would round toward
    // zero and produce the wrong cell for x in (-1, 0).
    return static_cast<int>(std::floor(coordinate / cellSize_));
}
