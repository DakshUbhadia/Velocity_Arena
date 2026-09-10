#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <cmath>

#include "physics/AABB.hpp"
#include "physics/Collision.hpp"
#include "physics/SpatialGrid.hpp"

// ── Helpers ──────────────────────────────────────────────────────────────────

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

static std::chrono::nanoseconds median(std::vector<std::chrono::nanoseconds>& samples)
{
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}

// ── Benchmark runner ─────────────────────────────────────────────────────────

struct WorkloadResult {
    std::size_t       entityCount{};
    std::uint64_t     baselineCandidates{};
    std::uint64_t     gridCandidates{};
    std::uint64_t     intersections{};
    std::chrono::nanoseconds baselineMedianTime{};
    std::chrono::nanoseconds gridMedianTime{};
    
    std::size_t       occupiedCells{};
    std::uint64_t     cellInsertions{};
    std::uint64_t     rawCellPairVisits{};
    std::uint64_t     duplicatePairsSkipped{};
    float             cellSize{};
};

static WorkloadResult runWorkload(std::size_t N, float cellSize, int repetitions = 5)
{
    // Fixed seed 1337 for reproducibility.
    std::mt19937 rng(1337);

    std::vector<AABB> boxes = generateScene(N, rng);
    std::span<const AABB> boxSpan(boxes);

    const std::uint64_t expectedCandidates =
        static_cast<std::uint64_t>(N) * (static_cast<std::uint64_t>(N) - 1) / 2;

    // ── Warm-up ───────────────────────────────────────────────────────────────
    // Warm-up is used to reduce first-run effects such as:
    // - cold instruction/data caches
    // - initial page residency
    // - branch predictor state
    // - one-time runtime/library effects
    CollisionStats warmupBase = bruteForceAllPairs(boxSpan);
    SpatialGridStats warmupGrid = spatialGridAllPairs(boxSpan, cellSize);

    if (warmupBase.candidateChecks != expectedCandidates) {
        std::cerr << "ERROR: N=" << N << " expected " << expectedCandidates << " candidates\n";
        std::exit(1);
    }
    if (warmupBase.intersections != warmupGrid.intersections) {
        std::cerr << "ERROR: N=" << N << " intersections mismatch: base=" 
                  << warmupBase.intersections << " grid=" << warmupGrid.intersections << "\n";
        std::exit(1);
    }

    // ── Timed repetitions ─────────────────────────────────────────────────────
    std::vector<std::chrono::nanoseconds> baseTimings;
    std::vector<std::chrono::nanoseconds> gridTimings;
    baseTimings.reserve(repetitions);
    gridTimings.reserve(repetitions);

    std::uint64_t lastIntersections = warmupBase.intersections;

    for (int rep = 0; rep < repetitions; ++rep) {
        // Run Baseline
        auto t0 = std::chrono::steady_clock::now();
        CollisionStats bStats = bruteForceAllPairs(boxSpan);
        auto t1 = std::chrono::steady_clock::now();
        baseTimings.push_back(t1 - t0);

        // Run Grid
        auto t2 = std::chrono::steady_clock::now();
        SpatialGridStats gStats = spatialGridAllPairs(boxSpan, cellSize);
        auto t3 = std::chrono::steady_clock::now();
        gridTimings.push_back(t3 - t2);

        if (bStats.intersections != lastIntersections || gStats.intersections != lastIntersections) {
            std::cerr << "ERROR: inconsistent intersection count on rep " << rep << "\n";
            std::exit(1);
        }
    }

    WorkloadResult result;
    result.entityCount    = N;
    result.baselineCandidates = expectedCandidates;
    result.gridCandidates = warmupGrid.candidateChecks;
    result.intersections   = lastIntersections;
    result.baselineMedianTime = median(baseTimings);
    result.gridMedianTime = median(gridTimings);
    result.cellSize = cellSize;
    result.occupiedCells = warmupGrid.occupiedCells;
    result.cellInsertions = warmupGrid.cellInsertions;
    result.rawCellPairVisits = warmupGrid.rawCellPairVisits;
    result.duplicatePairsSkipped = warmupGrid.duplicatePairsSkipped;
    return result;
}

// ── Main ─────────────────────────────────────────────────────────────────────

int main()
{
#ifndef NDEBUG
    std::cout << "WARNING: running in Debug build. Benchmark results will be inaccurate.\n\n";
#endif

    std::cout << "Velocity Arena -- Collision Baseline vs Spatial Grid\n"
              << "====================================================\n\n";

    const std::vector<std::size_t> entityCounts = {100, 500, 1000, 2500, 5000};
    const float mainCellSize = 2.0f;
    const int repetitions = 5;

    std::cout << "| N | Baseline Candidates | Grid Candidates | Reduction | Baseline us | Grid us | Speedup | Intersections |\n";
    std::cout << "|---|--------------------:|----------------:|----------:|------------:|--------:|--------:|--------------:|\n";

    std::vector<WorkloadResult> results;
    results.reserve(entityCounts.size());

    for (std::size_t N : entityCounts) {
        WorkloadResult r = runWorkload(N, mainCellSize, repetitions);
        results.push_back(r);

        double redPct = 100.0 * (1.0 - static_cast<double>(r.gridCandidates) / static_cast<double>(r.baselineCandidates));
        double b_us = static_cast<double>(r.baselineMedianTime.count()) / 1000.0;
        double g_us = static_cast<double>(r.gridMedianTime.count()) / 1000.0;
        double speedup = b_us / g_us;

        std::cout << "| " << std::left << std::setw(1) << N << " | ";
        std::cout << std::right << std::setw(19) << r.baselineCandidates << " | ";
        std::cout << std::setw(15) << r.gridCandidates << " | ";
        std::cout << std::setw(8) << std::fixed << std::setprecision(1) << redPct << "% | ";
        std::cout << std::setw(11) << static_cast<long long>(b_us) << " | ";
        std::cout << std::setw(7) << static_cast<long long>(g_us) << " | ";
        std::cout << std::setw(6) << std::fixed << std::setprecision(2) << speedup << "x | ";
        std::cout << std::setw(13) << r.intersections << " |\n";
    }

    std::cout << "\nCell-Size Sensitivity (N=5000):\n";
    std::cout << "| Cell Size | Candidates | Cell Insertions | Occupied Cells | Grid Time | Speedup |\n";
    std::cout << "|-----------|------------|-----------------|----------------|-----------|---------|\n";
    
    std::vector<float> sensitivitySizes = {1.0f, 2.0f, 4.0f};
    for (float cs : sensitivitySizes) {
        WorkloadResult r = runWorkload(5000, cs, repetitions);
        double b_us = static_cast<double>(r.baselineMedianTime.count()) / 1000.0;
        double g_us = static_cast<double>(r.gridMedianTime.count()) / 1000.0;
        double speedup = b_us / g_us;

        std::cout << "| " << std::left << std::setw(9) << std::fixed << std::setprecision(1) << cs << " | ";
        std::cout << std::right << std::setw(10) << r.gridCandidates << " | ";
        std::cout << std::setw(15) << r.cellInsertions << " | ";
        std::cout << std::setw(14) << r.occupiedCells << " | ";
        std::cout << std::setw(6) << static_cast<long long>(g_us) << " us | ";
        std::cout << std::setw(6) << std::fixed << std::setprecision(2) << speedup << "x |\n";
    }

    // ── CSV output ────────────────────────────────────────────────────────────
    std::filesystem::create_directories("results");
    const std::string csvPath = "results/collision_comparison.csv";

    std::ofstream csv(csvPath);
    if (csv) {
        csv << "entity_count,cell_size,baseline_candidates,grid_candidates,candidate_reduction_percent,"
            << "intersections,baseline_median_us,grid_median_us,speedup,time_reduction_percent,"
            << "occupied_cells,cell_insertions,raw_cell_pair_visits,duplicate_pairs_skipped\n";

        for (const auto& r : results) {
            double redPct = 100.0 * (1.0 - static_cast<double>(r.gridCandidates) / static_cast<double>(r.baselineCandidates));
            double b_us = static_cast<double>(r.baselineMedianTime.count()) / 1000.0;
            double g_us = static_cast<double>(r.gridMedianTime.count()) / 1000.0;
            double speedup = b_us / g_us;
            double timeRedPct = 100.0 * (1.0 - g_us / b_us);

            csv << r.entityCount << ","
                << r.cellSize << ","
                << r.baselineCandidates << ","
                << r.gridCandidates << ","
                << redPct << ","
                << r.intersections << ","
                << b_us << ","
                << g_us << ","
                << speedup << ","
                << timeRedPct << ","
                << r.occupiedCells << ","
                << r.cellInsertions << ","
                << r.rawCellPairVisits << ","
                << r.duplicatePairsSkipped << "\n";
        }
        csv.close();
        std::cout << "\nComparison results written to " << csvPath << "\n";
    }
    
    return 0;
}
