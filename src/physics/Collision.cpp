#include "physics/Collision.hpp"

CollisionStats bruteForceAllPairs(std::span<const AABB> boxes)
{
    // N*(N-1)/2 unique unordered pairs.
    // Starting j at i+1 avoids:
    //   (i, i) -- self-collision
    //   (j, i) -- duplicate of (i, j)
    //
    // candidateChecks will always equal N*(N-1)/2 after this loop.
    // This is used during benchmarking to verify correctness of the loop.

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
