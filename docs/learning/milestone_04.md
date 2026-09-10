# Milestone 4 -- AABB Collision and Performance Baseline

## What We Built

Milestone 4 introduces a reusable 3D Axis-Aligned Bounding Box (AABB) collision
primitive, integrates it into the gameplay loop, and establishes a deterministic
brute-force all-pairs collision benchmark that serves as the performance baseline
for the future spatial-grid optimization.

---

## Files Added

| File                               | Purpose                                            |
|------------------------------------|----------------------------------------------------|
| src/physics/AABB.hpp               | AABB class declaration                             |
| src/physics/AABB.cpp               | AABB construction and intersection implementation  |
| src/physics/Collision.hpp          | CollisionStats struct + bruteForceAllPairs API      |
| src/physics/Collision.cpp          | All-pairs brute-force implementation               |
| benchmarks/collision_benchmark.cpp | Deterministic Release-mode performance benchmark   |
| tests/aabb_tests.cpp               | 10 dependency-free AABB correctness tests          |
| docs/performance.md                | Baseline performance documentation                 |
| docs/learning/milestone_04.md      | This file                                          |

## Files Modified

| File                       | Change                                                          |
|----------------------------|-----------------------------------------------------------------|
| CMakeLists.txt             | Added velocity_physics library; gated networking/Catch2 deps   |
| src/game/Player.hpp/.cpp   | Added kHalfSize + bounds()                                      |
| src/game/Enemy.hpp/.cpp    | Added kHalfSize + bounds(); tuned attackRange to 1.0            |
| src/game/Projectile.hpp/.cpp | Added kSize/kHalfSize + bounds(); unified scale in modelMatrix |
| src/game/Game.cpp          | Replaced spherical collision with AABB; fixed spawn safety      |
| docs/mathematics.md        | Added AABB, interval overlap, complexity sections               |
| .gitignore                 | Added .deps/ (shared FetchContent cache)                        |

---

## Why Replace Distance-Based Collision

The original collision test was:
```cpp
float distSq = dx*dx + dy*dy + dz*dz;
if (distSq <= combinedRadiusSq) { ... }
```

Problems:
1. Magic numbers for enemy/projectile radii not tied to rendered sizes.
2. Spherical test is an approximation for cube-shaped objects.
3. Not reusable -- duplicated per-entity-pair type.
4. No shared abstraction for future optimization.

AABB replacement:
```cpp
if (proj.bounds().intersects(enemy.bounds())) { ... }
```

Benefits:
1. Exact fit for cube geometry (no approximation).
2. Reusable across any entity pair.
3. Visual size and collision size derived from the same constant.
4. Foundation for spatial-grid optimization in the next milestone.

---

## What Is an AABB?

An Axis-Aligned Bounding Box is the simplest 3D volume used in collision detection.

- "Axis-aligned" means the box sides are parallel to the world X, Y, Z axes.
- It is defined by two corner points: `min` and `max`.
- Every point `P` inside the box satisfies: `min <= P <= max` on all three axes.

Example: A unit cube centered at the origin has:
```
min = (-0.5, -0.5, -0.5)
max = ( 0.5,  0.5,  0.5)
```

---

## Center and Half Extents

Game code commonly describes objects using center + half-extents:
```
min = center - halfExtents
max = center + halfExtents
```

Half-extents (also called "radius" in some engines) are half the width/height/depth.
A 1x1x1 cube has half-extents (0.5, 0.5, 0.5).

The factory method `AABB::fromCenterHalfExtents` handles this conversion.

---

## Min and Max Coordinates

The min and max corners fully define the AABB.
Any point strictly between min and max is inside the box.
Points on the surface satisfy one or more coordinates equal to min or max.

---

## Interval Overlap

On a single axis, two AABBs overlap if their intervals overlap:
```
[A.min.x, A.max.x] overlaps [B.min.x, B.max.x]
iff
A.max.x >= B.min.x  AND  A.min.x <= B.max.x
```

The complement (separation):
```
A.max.x < B.min.x  (A entirely left of B)
OR
A.min.x > B.max.x  (A entirely right of B)
```

---

## AABB Intersection Algorithm

Early-out per-axis implementation:
```cpp
bool AABB::intersects(const AABB& other) const {
    if (max_.x < other.min_.x || min_.x > other.max_.x) return false;
    if (max_.y < other.min_.y || min_.y > other.max_.y) return false;
    if (max_.z < other.min_.z || min_.z > other.max_.z) return false;
    return true;
}
```

- If there is any axis with a gap, we immediately return false.
- We only reach `return true` if all three axes overlap.

---

## Touching Boundary Semantics

We use `>=` and `<=` (not `>` and `<`). This means:
- Two boxes touching face-to-face **are considered intersecting**.
- Two boxes touching edge-to-edge **are considered intersecting**.
- Two boxes touching corner-to-corner **are considered intersecting**.

This is the standard convention in game engines. Boundary contact is a
degenerate form of intersection, not separation.

---

## Gameplay Collision Integration

Game::resolveProjectileEnemyCollisions() was updated to use AABB:
```cpp
for (auto& proj : projectiles_) {
    if (!proj.active()) continue;
    for (auto& enemy : enemies_) {
        if (!enemy.alive()) continue;
        if (proj.bounds().intersects(enemy.bounds())) {
            proj.deactivate();
            enemy.kill();
            score_ += 100;
            break;
        }
    }
}
```

The outer loop is over P projectiles, inner loop over E enemies.
Complexity: O(P * E).

This is intentional for Milestone 4. The dedicated benchmark separately
tests a generalized N-body all-pairs search which is O(N^2).

---

## Naive All-Pairs Search

`bruteForceAllPairs` tests every unique unordered pair:
```cpp
for (size_t i = 0; i < N; ++i) {
    for (size_t j = i + 1; j < N; ++j) {
        ++stats.candidateChecks;
        if (boxes[i].intersects(boxes[j])) {
            ++stats.intersections;
        }
    }
}
```

Starting `j` at `i + 1`:
- Avoids (i, i) -- self-collision
- Avoids testing (j, i) after (i, j) -- no duplicates

---

## Candidate Pairs

A "candidate pair" is any pair (i, j) where i < j that we actually test.
The number of candidate pairs equals N*(N-1)/2 -- the number of unique
unordered pairs from a set of N elements.

In the spatial-grid milestone, this count will be dramatically reduced.
The ratio `spatialGrid.candidateChecks / bruteForce.candidateChecks`
will be our primary optimization metric.

---

## Deriving N(N-1)/2

N objects. We want to count unique unordered pairs {i, j} where i != j.

Step 1: Choose i: N options.
Step 2: Choose j: (N-1) options (anything except i).
Result: N*(N-1) ordered pairs (A,B) and (B,A) each counted.
Divide by 2 (since {A,B} and {B,A} are the same pair): N*(N-1)/2.

Verification:
```
N=100:  100 * 99 / 2 = 4,950
N=5000: 5000 * 4999 / 2 = 12,497,500
```

---

## O(1) AABB Test vs O(N^2) Pair Search

These two complexity claims are NOT contradictory:

| What                           | Complexity |
|--------------------------------|------------|
| One AABB::intersects() call    | O(1)       |
| All-pairs search for N objects | O(N^2)     |

The O(1) applies to a SINGLE test. The O(N^2) applies to the LOOP that
calls it N*(N-1)/2 times. Confusing these is a common interview mistake.

---

## Deterministic Benchmarking

Deterministic = same input produces same output every run.

The benchmark uses `std::mt19937 rng(1337)` -- a fixed seed.
This means:
- Scene generation produces the same AABBs every time.
- Intersection counts are reproducible.
- Results can be compared across machines and compiler versions.
- No "lucky" or "unlucky" benchmark runs due to random variation.

---

## Fixed Random Seed

`std::mt19937` is a Mersenne Twister PRNG. With seed 1337:
- The first call to the distribution produces the same value every time.
- The sequence is deterministic until the RNG state is exhausted.

Using `std::random_device` (truly random) would make results irreproducible.

---

## Why Scene Generation Is Outside Timing

The benchmark measures ONLY the collision detection cost. Scene generation
(memory allocation, random number generation, AABB construction) is a
separate concern not related to the collision algorithm.

Including scene generation in timed code would conflate two different
operations and produce numbers that are not comparable between implementations.

---

## Warm-Up Runs

The first call to `bruteForceAllPairs` is untimed. Reasons:
1. CPU instruction caches are cold on the first call.
2. Branch predictor hasn't seen the loop structure yet.
3. OS may page-fault on first memory access.

The warm-up primes these hardware effects so timed runs measure
steady-state performance.

---

## Median Runtime

We take 5 timed runs and report the median. Reasons:
- The minimum is too optimistic (lucky timing, no OS interrupts).
- The mean is skewed by outliers (OS task switches, cache evictions).
- The median is a robust central estimate that ignores occasional spikes.

---

## Release vs Debug Benchmarking

Debug builds include:
- No compiler optimizations (-O0).
- Extra assertions and safety checks.
- Potentially different code layout.

Debug performance numbers are 5-20x slower than Release for loop-heavy code.

All documented baseline numbers come from Release builds only.
The benchmark prints a WARNING when compiled without NDEBUG.

---

## Benchmark Results

Measured on WSL (Linux), Release build, Ninja, std::chrono::steady_clock, seed 1337.

| N    | Candidate Checks | Intersections | Median Time (us) | ns/Candidate |
|------|-----------------|---------------|------------------|--------------|
| 100  | 4,950           | 28            | 25               | 5.05         |
| 500  | 124,750         | 171           | 2,131            | 17.09        |
| 1000 | 499,500         | 325           | 6,936            | 13.89        |
| 2500 | 3,123,750       | 759           | 55,590           | 17.80        |
| 5000 | 12,497,500      | 1,637         | 169,939          | 13.60        |

Candidate counts match N*(N-1)/2 exactly for all workloads.
Time scales approximately as N^2 (O(N^2) confirmed empirically).

See full CSV: results/collision_baseline.csv

---

## C++ Concepts Used

| Concept                     | Where used                                           |
|-----------------------------|------------------------------------------------------|
| std::span                   | bruteForceAllPairs parameter (non-owning view)       |
| std::uint64_t               | Candidate/intersection counts (avoids overflow)      |
| std::mt19937                | Deterministic scene generation                       |
| std::chrono::steady_clock   | High-resolution timing                               |
| std::filesystem             | Creating results/ directory                          |
| constexpr static members    | kSize, kHalfSize in entity classes                   |
| assert()                    | Debug validation of AABB construction preconditions  |
| std::erase_if               | Clean removal of inactive objects                    |

---

## Design Decisions

1. **AABB as a value type** -- no virtual dispatch, no allocation overhead.
2. **Early-out intersection** -- returns false on first separating axis found.
3. **Shared kSize/kHalfSize constants** -- prevents visual/collision size drift.
4. **No spatial grid yet** -- this milestone establishes the baseline only.
5. **std::span for benchmark** -- avoids copying the vector, reads directly.
6. **velocity_physics static library** -- both game and benchmark use same code.

---

## Alternatives We Did Not Use

| Alternative           | Why Not                                                      |
|-----------------------|--------------------------------------------------------------|
| OBB (oriented box)    | Too complex for this stage; AABB sufficient for cube shapes  |
| Sphere collision      | Less accurate for cubes; already had this (replaced it)      |
| Collider base class   | Unnecessary abstraction overhead for this milestone          |
| PhysicsWorld class    | Scope creep; game doesn't need physics simulation           |
| Catch2 testing        | Network dependency caused build issues; custom tests simpler |
| Spatial grid          | Next milestone only; would invalidate baseline comparison    |

---

## Known Limitations

1. **AABB does not rotate** -- if entities rotate, the AABB must be recomputed
   from the rotated mesh extents or replaced with an OBB.
2. **No narrow phase** -- AABB intersection IS the collision check; there is no
   follow-up mesh-level test.
3. **Y axis not clamped** -- boxes can overlap vertically even if visually separate.
4. **No collision response** -- entities phase through each other (by design for now).

---

## Interview Questions

**Q1: What is an AABB?**
A: An Axis-Aligned Bounding Box is a rectangular 3D volume whose sides are parallel
   to the world X, Y, Z axes. It is defined by min and max corner points.

**Q2: Why is it called axis-aligned?**
A: Because all six faces are perpendicular to one of the three world axes.
   This constraint makes intersection testing trivial -- you only need 6 comparisons.

**Q3: How do you construct an AABB from center and half extents?**
A: min = center - halfExtents; max = center + halfExtents.
   Half extents must be non-negative (asserted in debug builds).

**Q4: How do you test whether two AABBs intersect?**
A: Check if intervals overlap on all three axes. If any axis has a gap (separating axis),
   return false immediately. If all three axes overlap, return true.

**Q5: Why must the boxes overlap on every axis?**
A: Separating Axis Theorem -- if there exists ANY axis where projections don't overlap,
   the shapes are separated in 3D, regardless of other axes.

**Q6: Is one AABB intersection O(1)?**
A: Yes. It requires at most 6 comparisons (2 per axis) with early-out.

**Q7: Why does brute-force collision become O(N^2)?**
A: For N objects, testing all unique pairs requires N*(N-1)/2 calls,
   which grows as N^2.

**Q8: Why is the exact number of unordered pairs N*(N-1)/2?**
A: N*(N-1) counts ordered pairs (A,B) and (B,A) separately. Dividing by 2
   gives unique unordered pairs.

**Q9: Why do we start j at i+1?**
A: To test each pair exactly once. If j started at 0, we would test both (A,B) and (B,A).

**Q10: Why don't we compare an object with itself?**
A: Self-intersection is trivially true (a box always intersects itself) and provides
   no useful information for collision detection.

**Q11: Why don't we test both (A,B) and (B,A)?**
A: Because intersects() is symmetric -- if A overlaps B, then B overlaps A.
   Testing both wastes work and double-counts the pair.

**Q12: Why did we replace spherical projectile collision with AABB?**
A: The spherical test used magic number radii not tied to rendered size.
   AABB is more accurate for cube geometry, reusable, and the visual/collision
   sizes are derived from the same constant (kSize).

**Q13: What are the weaknesses of AABB?**
A: (1) Does not handle rotation -- a rotated object may have a very loose AABB.
   (2) Can produce false positives for non-box shapes at corners.
   (3) Must be recomputed when object moves/rotates.

**Q14: What happens when an object rotates?**
A: The AABB must either be recomputed from the rotated mesh extents (remains axis-aligned,
   but becomes looser) or replaced with an OBB (more complex, tighter fit).

**Q15: Why is AABB especially simple for this game?**
A: All entities are axis-aligned unit cubes. Their AABB fits exactly with zero wasted space.
   No rotation means the AABB never needs to be expanded.

**Q16: What is broad-phase collision detection?**
A: A fast, approximate step that identifies candidate pairs likely to collide.
   Not all candidates actually intersect -- the broad phase reduces work for narrow phase.

**Q17: What is narrow-phase collision detection?**
A: An accurate, expensive test (e.g., mesh-level, SAT) applied to candidate pairs
   identified by the broad phase.

**Q18: What does candidate pair mean?**
A: A pair of objects that the broad phase selected for further testing. They may or may
   not actually intersect.

**Q19: Why create a brute-force baseline before optimizing?**
A: Without a baseline, you cannot measure whether an optimization actually improved
   performance. Optimization claims require evidence from comparison.

**Q20: Why use deterministic benchmark input?**
A: So that results are reproducible -- same seed produces same scene.
   This allows fair comparison between implementations and across machines.

**Q21: Why seed mt19937 with a fixed seed?**
A: To get a deterministic sequence. Different seeds produce different distributions;
   a fixed seed ensures every benchmark run uses identical input data.

**Q22: Why is random generation excluded from timed code?**
A: We want to measure only the collision detection algorithm cost, not the scene
   setup cost. Mixing them would produce numbers that don't compare cleanly.

**Q23: Why perform a warm-up?**
A: To prime CPU instruction caches, branch predictor, and TLB before timing.
   The first run is often slower due to cold-start effects.

**Q24: Why use median rather than one measurement?**
A: A single measurement may be an outlier (OS interrupt, cache miss, etc.).
   The median of 5 runs is a robust estimate that ignores occasional noise.

**Q25: Why benchmark a Release build?**
A: Debug builds are ~5-20x slower due to disabled optimizations and extra checks.
   Only Release performance is representative of production behavior.

**Q26: What causes timing noise?**
A: OS task switches, CPU frequency scaling, cache evictions by other processes,
   branch mispredictions, and memory bus contention.

**Q27: Why maintain approximately constant scene density?**
A: So workloads at different N are comparable. Without density scaling, larger N
   would produce denser scenes with more intersections, making times increase
   faster than pure O(N^2) and confusing the analysis.

**Q28: What metrics are recorded?**
A: Entity count, candidate checks, actual intersections, median time (microseconds),
   nanoseconds per candidate check.

**Q29: Why can candidate checks be a better optimization metric than FPS?**
A: FPS is affected by rendering, audio, physics, and many other systems.
   Candidate check count isolates the collision detection work specifically.
   It's also independent of CPU speed, making it a hardware-agnostic metric.

**Q30: How will a spatial grid improve this in the next optimization milestone?**
A: The grid divides world space into cells. Each object is assigned to a cell.
   Collision tests only happen between objects in the same or adjacent cells.
   For sparse scenes, most cells are empty, dramatically reducing candidate pairs.
   The reduction factor becomes our key optimization metric: bruteChecks / gridChecks.
