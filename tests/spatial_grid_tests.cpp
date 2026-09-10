#include <cstdio>
#include <cstdlib>
#include <vector>
#include <random>
#include <stdexcept>
#include <iostream>

#include "physics/AABB.hpp"
#include "physics/Collision.hpp"
#include "physics/SpatialGrid.hpp"

// ── Minimal test infrastructure ───────────────────────────────────────────────

static int g_passed = 0;
static int g_failed = 0;

static void check(bool condition, const char* testName) {
    if (condition) {
        std::printf("[PASS] %s\n", testName);
        ++g_passed;
    } else {
        std::printf("[FAIL] %s\n", testName);
        ++g_failed;
    }
}

// Helper to generate deterministic AABBs
static std::vector<AABB> generateScene(std::size_t N, std::mt19937& rng)
{
    const float worldSide = std::sqrt(static_cast<float>(N)) * 2.5f;
    const float halfWorld = worldSide * 0.5f;
    std::uniform_real_distribution<float> xzDist(-halfWorld, halfWorld);

    std::vector<AABB> boxes;
    boxes.reserve(N);

    for (std::size_t i = 0; i < N; ++i) {
        float cx = xzDist(rng);
        float cz = xzDist(rng);
        glm::vec3 center(cx, 0.5f, cz);
        glm::vec3 halfExtents(0.5f, 0.5f, 0.5f);
        boxes.push_back(AABB::fromCenterHalfExtents(center, halfExtents));
    }
    return boxes;
}

static void runPropertyTest(std::size_t N, unsigned int seed) {
    std::mt19937 rng(seed);
    auto boxes = generateScene(N, rng);
    auto brute = bruteForceAllPairs(boxes);
    auto grid = spatialGridAllPairs(boxes, 2.0f);

    bool ok = (brute.intersections == grid.intersections) && 
              (grid.candidateChecks <= brute.candidateChecks);
    
    char name[128];
    std::snprintf(name, sizeof(name), "Property Test N=%zu seed=%u", N, seed);
    check(ok, name);
    if (!ok) {
        std::printf("  Expected %llu intersections, got %llu\n", (unsigned long long)brute.intersections, (unsigned long long)grid.intersections);
    }
}

static void runQueryTests() {
    std::printf("Running SpatialGrid Query tests...\n\n");
    SpatialGrid grid(2.0f);
    std::vector<std::size_t> out;

    // Q1: Empty grid query
    grid.query(AABB(glm::vec3(0,0,0), glm::vec3(1,1,1)), out);
    check(out.empty(), "Query 1: Empty grid");

    // Q2: One object returned
    grid.insert(42, AABB(glm::vec3(0,0,0), glm::vec3(1,1,1)));
    grid.query(AABB(glm::vec3(0,0,0), glm::vec3(1,1,1)), out);
    check(out.size() == 1 && out[0] == 42, "Query 2: One object");

    // Q3: Non-overlapping returns empty
    grid.query(AABB(glm::vec3(10,0,10), glm::vec3(11,1,11)), out);
    check(out.empty(), "Query 3: Non-overlapping");

    // Q4: Multi-cell object returned once
    grid.insert(99, AABB(glm::vec3(0.5f,0,0.5f), glm::vec3(2.5f,1,2.5f))); // spans cells
    grid.query(AABB(glm::vec3(0.5f,0,0.5f), glm::vec3(2.5f,1,2.5f)), out);
    check(out.size() == 2 && out[0] == 42 && out[1] == 99, "Query 4: Multi-cell object returned once");
}

int main() {
    runQueryTests();
    std::printf("\nRunning SpatialGrid tests...\n\n");

    // TEST 1: Empty vector
    {
        std::vector<AABB> boxes;
        auto stats = spatialGridAllPairs(boxes, 2.0f);
        check(stats.candidateChecks == 0 && stats.intersections == 0, "Test 1: Empty vector");
    }

    // TEST 2: Single AABB
    {
        std::vector<AABB> boxes = { AABB(glm::vec3(0,0,0), glm::vec3(1,1,1)) };
        auto stats = spatialGridAllPairs(boxes, 2.0f);
        check(stats.candidateChecks == 0 && stats.intersections == 0, "Test 2: Single AABB");
    }

    // TEST 3: Two overlapping boxes in same cell
    {
        std::vector<AABB> boxes = {
            AABB(glm::vec3(0,0,0), glm::vec3(1,1,1)),
            AABB(glm::vec3(0.5f,0.5f,0.5f), glm::vec3(1.5f,1.5f,1.5f))
        };
        auto stats = spatialGridAllPairs(boxes, 2.0f);
        check(stats.intersections == 1, "Test 3: Overlapping in same cell");
    }

    // TEST 4: Two non-overlapping distant boxes
    {
        std::vector<AABB> boxes = {
            AABB(glm::vec3(0,0,0), glm::vec3(1,1,1)),
            AABB(glm::vec3(10,0,10), glm::vec3(11,1,11))
        };
        auto stats = spatialGridAllPairs(boxes, 2.0f);
        check(stats.intersections == 0, "Test 4: Non-overlapping distant boxes");
    }

    // TEST 5: Overlapping boxes straddling a cell boundary (e.g. boundary at x=2)
    {
        std::vector<AABB> boxes = {
            AABB(glm::vec3(1.6f,0,0), glm::vec3(2.2f,1,1)),
            AABB(glm::vec3(2.1f,0,0), glm::vec3(2.8f,1,1))
        };
        auto stats = spatialGridAllPairs(boxes, 2.0f);
        check(stats.intersections == 1, "Test 5: Overlapping straddling boundary");
    }

    // TEST 6: Negative world coordinates
    {
        std::vector<AABB> boxes = {
            AABB(glm::vec3(-1.0f,0,-1.0f), glm::vec3(-0.5f,1,-0.5f)),
            AABB(glm::vec3(-0.8f,0,-0.8f), glm::vec3(-0.2f,1,-0.2f))
        };
        auto stats = spatialGridAllPairs(boxes, 2.0f);
        check(stats.intersections == 1, "Test 6: Negative world coordinates");
    }

    // TEST 7: Touching AABBs on a grid boundary
    {
        std::vector<AABB> boxes = {
            AABB(glm::vec3(1,0,0), glm::vec3(2,1,1)), // max x = 2
            AABB(glm::vec3(2,0,0), glm::vec3(3,1,1))  // min x = 2
        };
        auto stats = spatialGridAllPairs(boxes, 2.0f);
        check(stats.intersections == 1, "Test 7: Touching AABBs on boundary");
    }

    // TEST 8: One AABB spanning multiple cells
    {
        SpatialGrid grid(1.0f);
        // Spans x from 0.5 to 2.5 (cells 0, 1, 2)
        grid.insert(0, AABB(glm::vec3(0.5f,0,0.5f), glm::vec3(2.5f,1,0.8f)));
        check(grid.occupiedCellCount() == 3, "Test 8: One AABB spanning multiple cells");
    }

    // TEST 9: Duplicate candidate suppression
    {
        std::vector<AABB> boxes = {
            AABB(glm::vec3(0.5f,0,0.5f), glm::vec3(2.5f,1,2.5f)),
            AABB(glm::vec3(0.5f,0,0.5f), glm::vec3(2.5f,1,2.5f))
        };
        auto stats = spatialGridAllPairs(boxes, 1.0f);
        // Will share multiple cells (x:0..2, z:0..2 = 9 cells total)
        // rawCellPairVisits should be > 1, candidateChecks should be exactly 1
        check(stats.rawCellPairVisits > 1 && stats.candidateChecks == 1 && stats.intersections == 1, "Test 9: Duplicate candidate suppression");
    }

    // TEST 10: Three objects, mixed overlap
    {
        std::vector<AABB> boxes = {
            AABB(glm::vec3(0,0,0), glm::vec3(1,1,1)),
            AABB(glm::vec3(0.5f,0,0.5f), glm::vec3(1.5f,1,1.5f)),
            AABB(glm::vec3(5,0,5), glm::vec3(6,1,6))
        };
        auto stats = spatialGridAllPairs(boxes, 2.0f);
        check(stats.intersections == 1 && stats.candidateChecks >= 1, "Test 10: Three objects mixed overlap");
    }

    // TEST 11 & 12: Deterministic random scenes
    {
        std::mt19937 rng1(1337);
        auto boxes1 = generateScene(100, rng1);
        auto b1 = bruteForceAllPairs(boxes1);
        auto g1 = spatialGridAllPairs(boxes1, 2.0f);
        check(b1.intersections == g1.intersections, "Test 11: Random N=100");

        std::mt19937 rng2(1337);
        auto boxes2 = generateScene(500, rng2);
        auto b2 = bruteForceAllPairs(boxes2);
        auto g2 = spatialGridAllPairs(boxes2, 2.0f);
        check(b2.intersections == g2.intersections, "Test 12: Random N=500");
    }

    // TEST 13 & 14: Invalid cell sizes
    {
        bool caught1 = false, caught2 = false;
        try { SpatialGrid grid(0.0f); } catch (const std::invalid_argument&) { caught1 = true; }
        try { SpatialGrid grid(-1.0f); } catch (const std::invalid_argument&) { caught2 = true; }
        check(caught1, "Test 13: Invalid cell size 0 rejects");
        check(caught2, "Test 14: Negative cell size rejects");
    }

    // Property correctness tests
    unsigned int seeds[] = {1, 42, 1337, 2026};
    std::size_t Ns[] = {20, 100, 250};
    for (auto N : Ns) {
        for (auto s : seeds) {
            runPropertyTest(N, s);
        }
    }

    std::printf("\nResults: %d passed, %d failed.\n", g_passed, g_failed);
    if (g_failed > 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
