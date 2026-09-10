// collision_benchmark.cpp
//
// Standalone brute-force AABB collision benchmark.
//
// Purpose:
//   Establishes a deterministic O(N^2) all-pairs collision baseline.
//   Future milestone will compare a spatial-grid broad phase against
//   these exact same workloads and metrics.
//
// Methodology:
//   - Fixed RNG seed (1337) for reproducibility across runs.
//   - Scene generated OUTSIDE the timed section.
//   - One untimed warm-up call before timed repetitions.
//   - 5 timed repetitions per workload; median is reported.
//   - Candidate count verified to equal N*(N-1)/2.
//   - Release-build warning printed when NDEBUG is not defined.
//   - Results written to results/collision_baseline.csv.
//
// Build:
//   cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON
//   cmake --build build-release
//   ./build-release/collision_benchmark

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <span>
#include <vector>

#include "physics/AABB.hpp"
#include "physics/Collision.hpp"

// ── Helpers ──────────────────────────────────────────────────────────────────

// Generate a reproducible set of N AABBs on the XZ plane.
//
// World-size scales with sqrt(N) so object density stays approximately
// constant as N grows. Without this scaling, larger N would produce
// artificially denser scenes with more intersections.
//
//   worldSide = sqrt(N) * 2.5
//   x in [-worldSide/2, worldSide/2]
//   z in [-worldSide/2, worldSide/2]
//   y center = 0.5, halfExtents = (0.5, 0.5, 0.5)
//
// The RNG is seeded externally so it can be reset between workloads
// for consistent per-workload sequences.
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

// Return the median of a vector of durations (modifies the vector in place via sort).
static std::chrono::nanoseconds median(std::vector<std::chrono::nanoseconds>& samples)
{
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}

// ── Benchmark runner ─────────────────────────────────────────────────────────

struct WorkloadResult {
    std::size_t       entityCount{};
    std::uint64_t     candidateChecks{};
    std::uint64_t     intersections{};
    std::chrono::nanoseconds medianTime{};
};

static WorkloadResult runWorkload(std::size_t N, int repetitions = 5)
{
    // Use a fixed seed for reproducibility.
    // Reset to 1337 for every workload so each N gets the same sequence.
    std::mt19937 rng(1337);

    // Generate scene OUTSIDE the timed section.
    std::vector<AABB> boxes = generateScene(N, rng);
    std::span<const AABB> boxSpan(boxes);

    // Expected candidate count: N*(N-1)/2
    const std::uint64_t expectedCandidates =
        static_cast<std::uint64_t>(N) * (static_cast<std::uint64_t>(N) - 1) / 2;

    // ── Warm-up ───────────────────────────────────────────────────────────────
    // One untimed warm-up run to populate CPU caches and JIT compile paths.
    CollisionStats warmupStats = bruteForceAllPairs(boxSpan);

    // Verify candidate count is correct.
    if (warmupStats.candidateChecks != expectedCandidates) {
        std::cerr << "ERROR: N=" << N
                  << " expected " << expectedCandidates
                  << " candidate checks but got " << warmupStats.candidateChecks
                  << "\n";
        std::exit(1);
    }

    // ── Timed repetitions ─────────────────────────────────────────────────────
    std::vector<std::chrono::nanoseconds> timings;
    timings.reserve(static_cast<std::size_t>(repetitions));

    std::uint64_t lastIntersections = warmupStats.intersections;

    for (int rep = 0; rep < repetitions; ++rep) {
        auto t0    = std::chrono::steady_clock::now();
        CollisionStats stats = bruteForceAllPairs(boxSpan);
        auto t1    = std::chrono::steady_clock::now();

        timings.push_back(t1 - t0);

        // Verify consistency across runs.
        if (stats.candidateChecks != expectedCandidates) {
            std::cerr << "ERROR: inconsistent candidate count on rep " << rep << "\n";
            std::exit(1);
        }
        if (stats.intersections != lastIntersections) {
            std::cerr << "ERROR: inconsistent intersection count on rep " << rep << "\n";
            std::exit(1);
        }
        lastIntersections = stats.intersections;
    }

    WorkloadResult result;
    result.entityCount    = N;
    result.candidateChecks = expectedCandidates;
    result.intersections   = lastIntersections;
    result.medianTime      = median(timings);
    return result;
}

// ── Main ─────────────────────────────────────────────────────────────────────

int main()
{
#ifndef NDEBUG
    std::cout << "WARNING: benchmark is not running in an optimized Release build.\n"
              << "         Performance numbers from this run should NOT be used\n"
              << "         for documentation. Rebuild with -DCMAKE_BUILD_TYPE=Release.\n\n";
#endif

    std::cout << "Velocity Arena -- AABB Collision Baseline Benchmark\n"
              << "====================================================\n\n";

    std::cout << "Configuration:\n"
              << "  RNG seed:     1337 (fixed for reproducibility)\n"
              << "  AABB size:    1x1x1 (half-extents 0.5)\n"
              << "  Scene scale:  worldSide = sqrt(N) * 2.5\n"
              << "  Warm-up:      1 untimed run\n"
              << "  Repetitions:  5 timed runs, median reported\n\n";

    // Entity counts to benchmark
    const std::vector<std::size_t> entityCounts = {100, 500, 1000, 2500, 5000};
    const int repetitions = 5;

    // Print table header
    std::cout << "| N      | Candidate Checks | Intersections | Median Time (us) | ns/Candidate |\n";
    std::cout << "|--------|-----------------|---------------|------------------|--------------|\n";

    std::vector<WorkloadResult> results;
    results.reserve(entityCounts.size());

    for (std::size_t N : entityCounts) {
        std::cout << "Running N=" << N << "..." << std::flush;

        WorkloadResult r = runWorkload(N, repetitions);
        results.push_back(r);

        double us         = static_cast<double>(r.medianTime.count()) / 1000.0;
        double nsPerCheck = (r.candidateChecks > 0)
            ? static_cast<double>(r.medianTime.count()) / static_cast<double>(r.candidateChecks)
            : 0.0;

        std::cout << "\r| " << std::left;
        std::cout.width(6); std::cout << N;
        std::cout << " | ";
        std::cout.width(15); std::cout << r.candidateChecks;
        std::cout << " | ";
        std::cout.width(13); std::cout << r.intersections;
        std::cout << " | ";
        std::cout.width(16); std::cout << static_cast<long long>(us);
        std::cout << " | ";
        std::cout.width(12); std::cout << nsPerCheck;
        std::cout << " |\n";
    }

    // ── CSV output ────────────────────────────────────────────────────────────
    std::filesystem::create_directories("results");
    const std::string csvPath = "results/collision_baseline.csv";

    std::ofstream csv(csvPath);
    if (!csv) {
        std::cerr << "ERROR: Could not open " << csvPath << " for writing.\n";
        return 1;
    }

    csv << "entity_count,candidate_checks,intersections,median_time_us,ns_per_candidate\n";

    for (const auto& r : results) {
        double us = static_cast<double>(r.medianTime.count()) / 1000.0;
        double nsPerCheck = (r.candidateChecks > 0)
            ? static_cast<double>(r.medianTime.count()) / static_cast<double>(r.candidateChecks)
            : 0.0;

        csv << r.entityCount    << ","
            << r.candidateChecks << ","
            << r.intersections   << ","
            << us                << ","
            << nsPerCheck        << "\n";
    }

    csv.close();
    std::cout << "\nBaseline results written to " << csvPath << "\n";

    return 0;
}
