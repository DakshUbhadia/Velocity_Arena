# Collision Performance Baseline

## Why We Need a Baseline

You cannot claim an optimization improved performance without first measuring
the unoptimized implementation. This document records the brute-force all-pairs
AABB collision baseline so that the future spatial-grid milestone can present
objective, reproducible comparisons.

Without a baseline:
- "2x faster" is meaningless.
- "reduced candidate pairs by 80%" cannot be verified.
- Benchmark results depend on environment and could be fabricated.

With a deterministic baseline, the next milestone can say:
  "For N = 5000, brute force checks 12,497,500 pairs in X us.
   The spatial grid checks Y pairs in Z us — an N% reduction and M% speedup."

---

## AABB Cost

A single AABB-vs-AABB intersection test requires a constant number of
comparisons (6 comparisons in the worst case, fewer with early-out).

**A single AABB intersection test is O(1).**

This does NOT mean the overall collision system is O(1). The O(1) label
applies only to the individual test, not to the loop that calls it.

---

## Naive Broad Phase

When there are N objects, testing every unique pair requires:

    N * (N - 1) / 2

candidate checks.

Asymptotically this is **O(N^2)**.

Example counts:
| N    | Candidate Pairs           |
|------|--------------------------|
| 100  | 4,950                    |
| 500  | 124,750                  |
| 1000 | 499,500                  |
| 2500 | 3,123,750                |
| 5000 | 12,497,500               |

The game's gameplay collision loop (Projectiles x Enemies) is separate:
it is O(P * E) and is not benchmarked here. This benchmark tests
a generalized N-body all-pairs search.

---

## Benchmark Workload

| Parameter        | Value                                      |
|------------------|--------------------------------------------|
| RNG seed         | 1337 (fixed for reproducibility)           |
| Entity counts    | 100, 500, 1000, 2500, 5000                 |
| AABB size        | 1 x 1 x 1 unit cube (half-extents 0.5)    |
| World size rule  | worldSide = sqrt(N) * 2.5                  |
| X/Z distribution | Uniform on [-worldSide/2, worldSide/2]     |
| Y center         | 0.5 (constant, all boxes on ground plane)  |
| Scene generation | OUTSIDE the timed section                  |
| Warm-up          | 1 untimed call before timed repetitions    |
| Repetitions      | 5 timed runs per workload                  |
| Reported metric  | Median of the 5 runs                       |
| Build type       | Release (-O2 or equivalent)                |
| Clock            | std::chrono::steady_clock                  |

The world-size scaling (`sqrt(N) * 2.5`) ensures object density stays
approximately constant as N grows. Without it, larger N would produce
artificially denser scenes with more intersections, making the workloads
incomparable.

---

## Baseline Results

*Results will be populated after running the Release benchmark:*
```
./build-release/collision_benchmark
```

The benchmark writes results to `results/collision_baseline.csv`.

| N    | Candidate Checks | Intersections | Median Time (us) | ns/Candidate |
|------|-----------------|---------------|------------------|--------------|
| 100  | 4,950           | 28            | 24.98            | 5.05         |
| 500  | 124,750         | 171           | 2,131            | 17.09        |
| 1000 | 499,500         | 325           | 6,936            | 13.89        |
| 2500 | 3,123,750       | 759           | 55,590           | 17.80        |
| 5000 | 12,497,500      | 1,637         | 169,939          | 13.60        |

Measured on WSL (Linux) Release build with Ninja, std::chrono::steady_clock.
Fixed seed 1337. worldSide = sqrt(N) * 2.5.

---

## Observations

The data shows clearly O(N^2) growth behavior:

- From N=100 to N=500 (5x more entities): time grows ~85x (24us -> 2131us).
  Approximately 5^2 = 25x expected; actual is somewhat higher, possibly due to
  cache effects at larger working sets.
- From N=500 to N=1000 (2x more entities): time grows ~3.3x (2131us -> 6936us).
  Approximately 2^2 = 4x expected; close to theoretical.
- From N=1000 to N=5000 (5x more entities): time grows ~24.5x (6936us -> 169939us).
  Approximately 5^2 = 25x expected; very close to theoretical O(N^2) scaling.

The ns/candidate metric oscillates between 5-18 ns, reflecting CPU cache behavior.
For small N the dataset fits in L1/L2 cache; for large N cache misses dominate.

Candidate counts match the N*(N-1)/2 formula exactly for all workloads,
confirming correct implementation of the loop bounds.

---

## Next Optimization

A uniform spatial grid will be introduced in Milestone 5.

The grid partitions space into cells. Objects only need to test against
objects in the same or adjacent cells, dramatically reducing the number
of candidate pairs for sparse-to-medium density scenes.

The next milestone will run the exact same entity counts (100, 500, 1000,
2500, 5000) with the exact same fixed seed (1337) and compare:
- candidate checks (brute-force vs spatial-grid)
- elapsed time (brute-force vs spatial-grid)
- intersection count (must match -- same scene, same objects)

# Uniform Spatial Grid Broad Phase

## Problem

Brute-force all-pairs generates:

    N(N-1)/2

candidate checks. For N=5000 this leads to over 12 million AABB intersection tests, dropping performance drastically.

## Approach

A Uniform Spatial Grid was implemented to act as a broad phase collision detection step:
- **fixed-size XZ cells**: Partition the playfield uniformly into square cells.
- **AABB multi-cell insertion**: Map AABBs using standard mathematical conversion into all overlapped cells to prevent missed cross-cell boundaries.
- **candidate generation inside cells**: Only objects grouped in the same cell undergo candidate verification.
- **duplicate candidate suppression**: Standardize bounding object index pairs and deduplicate via an unordered set to ensure unique tests.
- **same AABB narrow-phase**: The final testing evaluates exactly the same intersection primitive.

## Cell Mapping

Cell assignment leverages pure continuous float mapping:

    floor(position / cellSize)

The use of `std::floor` is extremely critical. Normal integer truncation will round `-0.5` up to `0`, causing erratic and incorrect binning for negative coordinates. Using `floor` ensures negative segments map to continuous adjacent spatial grid indices correctly.

## Complexity

The spatial grid has average expected behavior:

    O(N + K)

where K depends heavily on occupancy of cells. However, its worst-case scenario remains:

    O(N^2)

For instance, if all objects fall into the exact same cell, all candidates must be verified. Unordered container operations (hash map/set insertion) also rely on average O(1) performance and are not strictly guaranteed worst-case O(1).

## Benchmark Fairness

Our methodology ensures that both baseline and optimized tests operate on even ground:
- identical AABBs generated per phase
- same fixed seed (1337)
- same Release build flag (-O2)
- same repetition count
- same measurement session directly sequentially to equalize CPU clock scheduling / thermal differences.

## Results

| N | Baseline Candidates | Grid Candidates | Reduction | Baseline us | Grid us | Speedup | Intersections |
|---|--------------------:|----------------:|----------:|------------:|--------:|--------:|--------------:|
| 100 |                4950 |             114 |     97.7% |          15 |      35 |   0.45x |            28 |
| 500 |              124750 |             599 |     99.5% |         593 |     224 |   2.65x |           171 |
| 1000 |              499500 |            1249 |     99.7% |        2234 |     542 |   4.12x |           325 |
| 2500 |             3123750 |            3215 |     99.9% |       15801 |    1456 |  10.85x |           759 |
| 5000 |            12497500 |            6619 |     99.9% |       69514 |    2780 |  25.00x |          1637 |

## Candidate Reduction

At N=5000, candidate checks dropped from 12,497,500 down to merely 6,619. This is a massive 99.9% measured reduction in actual overlap tests required.

## Runtime Reduction

With N=5000, overall end-to-end grid time measured at 2780 us compared to baseline 69514 us, yielding an approximately 25x runtime speedup, representing around 96% measured end-to-end runtime reduction.

## Crossover Point

At small counts (e.g. N=100), brute force can perform significantly faster. In this setup, Baseline outperformed Grid by roughly 2x (15 us vs 35 us). The overhead incurred by unordered maps, grid creation, allocation, and hashing overcomes the benefits of eliminating merely ~4000 checks.

## Cell-Size Sensitivity

| Cell Size | Candidates | Cell Insertions | Occupied Cells | Grid Time | Speedup |
|-----------|------------|-----------------|----------------|-----------|---------|
| 1.0       |       3658 |           20000 |          14692 |   4259 us |  17.28x |
| 2.0       |       6619 |           11435 |           6029 |   2575 us |  26.00x |
| 4.0       |      14437 |            7887 |           2004 |   2256 us |  29.11x |

In this test, the 4.0 cell size showed the quickest runtime due to having far fewer occupied cells to iterate and insertions to perform, while effectively avoiding excessive candidate testing compared to the baseline.

## Trade-offs

Grid advantages:
- far fewer candidate pairs
- strong spatial locality
- highly scalable for sparse-world collision

Grid costs:
- hash table / dynamic allocation overhead
- requires memory allocations and map operations
- cell insertion computation complexity
- requires duplicate suppression passes
- requires fine tuning cell size

## Worst Case

If all objects are pushed to the corner of the map or the exact same spot (i.e. all falling into a single cell), the algorithm must still verify all `N(N-1)/2` checks while absorbing the additional hit of hashing, mapping, and iterating unordered maps. This results in standard O(N^2) complexity with extra baseline overhead.
