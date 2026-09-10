# Velocity Arena - Progress Tracker

## Milestone 1 - Basic Engine Setup

**Status**: Completed and Verified

**What has been implemented:**
- Basic CMake setup for C++20.
- Window management with GLFW.
- OpenGL context setup with GLAD.
- Math operations using GLM.
- Basic rendering abstractions: `Shader`, `Mesh`, `Renderer`.
- Rendering of a floor plane and multi-colored cubes.
- Projection matrix responding to window resizing.
- Depth testing.
- Simple escape-to-close behavior.

## Milestone 2 - Camera, Input and Delta-Time Movement

**Status**: Completed and Verified

**What has been implemented:**
- Added `GameClock` using `std::chrono::steady_clock` for per-frame delta-time measurement.
- Added lightweight GLFW-backed `Input` abstraction.
- Added reusable `Camera` abstraction using `glm::lookAt`.
- Added minimal controllable `Player` entity.
- Implemented WASD movement on the XZ arena plane.
- Implemented frame-rate-independent movement using delta time.
- Normalized movement vectors to prevent faster diagonal movement.
- Added simple arena boundary constraints.
- Replaced the fixed Milestone 1 camera with a third-person follow camera.
- Extended 3D mathematics documentation with vector magnitude, normalization and movement equations.
- Added Milestone 2 architecture and learning documentation.

**Verification:**
- Debug build configured successfully.
- `velocity_arena` compiled successfully.
- Application launched successfully.
- WASD movement verified.
- Diagonal movement normalization verified.
- Camera-follow behavior verified.
- Arena boundary behavior verified.
- Window resize behavior remains functional.
- Depth testing remains functional.
- Escape-to-close remains functional.

## Milestone 3 - Core Gameplay Loop

**Status**: Completed and Verified
Sanitizer verification pending due to CMake/FetchContent network issue during dependency download.

**What has been implemented:**
- Added a central `Game` class owning Player, Enemy, and Projectile gameplay state.
- Extended Player with health and persistent normalized facing direction.
- Added projectile spawning, delta-time movement, finite lifetime, and controlled firing cooldown.
- Added Enemy entities using an Idle/Chase/Attack finite-state machine.
- Added normalized enemy pursuit and cooldown-based enemy attacks.
- Added randomized perimeter enemy spawning with bounded spawn safety.
- Added temporary squared-distance projectile/enemy collision detection.
- Added score tracking, player damage, game-over state, and restart.
- Reused the existing cube mesh for Player, Enemy, and Projectile rendering.
- Extended window-title information with health, score, enemy count, game state, and FPS.
- Expanded mathematics, architecture, and learning documentation.

**Verification:**
- Debug configuration completed successfully.
- Application compiled successfully.
- Application launched successfully.
- WASD/camera behavior remains functional.
- Projectile shooting verified.
- Enemy spawning/FSM behavior verified.
- Enemy attacks/player health verified.
- Projectile/enemy hits and scoring verified.
- Game-over/restart behavior verified.
- Resize/minimize handling verified.
- Sanitizer run could not complete due to persistent CMake/FetchContent network hangs during dependency download.

## Milestone 4 - AABB Collision and Performance Baseline

**Status**: Completed and Verified

**What has been implemented:**
- Added reusable 3D Axis-Aligned Bounding Box (`AABB`) collision primitive using
  interval-overlap testing on all three axes with early-out per-axis separation.
- Added collision bounds (`bounds()`) for `Player`, `Enemy`, and `Projectile` entities.
- Used named constants (`kHalfSize`, `kSize`) to keep rendered size and collision
  size derived from the same source -- preventing silent divergence.
- Replaced temporary projectile/enemy radius collision with `proj.bounds().intersects(enemy.bounds())`,
  preserving all existing gameplay behavior (O(P*E) complexity).
- Fixed `spawnEnemy()` spawn-safety check: `distSq >= kMinSpawnDistance * kMinSpawnDistance`
  (was incorrectly comparing squared distance against a non-squared threshold of 2.5).
- Fixed `Game` constructor and `reset()` to set `spawnTimer_ = spawnInterval_` after
  initial enemy spawns, preventing immediate additional spawns.
- Added `bruteForceAllPairs()` function with `CollisionStats` (candidateChecks, intersections)
  as a deterministic O(N^2) all-pairs baseline.
- Added shared `velocity_physics` static library used by both `velocity_arena` and the benchmark.
- Added standalone `collision_benchmark` (Release mode) covering N = 100, 500, 1000, 2500, 5000.
- Used fixed RNG seed (1337) for deterministic benchmark scenes.
- Applied world-size scaling (worldSide = sqrt(N) * 2.5) to maintain constant object density.
- Added warm-up call, 5 timed repetitions, and median timing per workload.
- CSV output to `results/collision_baseline.csv`.
- Added dependency-free AABB correctness tests (10 test cases) via CTest.
- Gated `nlohmann/json` and `cpp-httplib` behind `ENABLE_NETWORKING=ON` option.
- Added shared `.deps/` FetchContent base directory to reduce redownloads across build dirs.
- Added `docs/performance.md` with baseline methodology and actual measured results.
- Extended `docs/mathematics.md` with AABB, interval overlap, and complexity sections.
- Added `docs/learning/milestone_04.md` with full learning narrative and 30 interview Q&As.

**Verification:**
- Debug build completed successfully (0 errors, 0 warnings).
- AABB correctness tests passed: 14/14 (CTest: 100% passed).
- All candidate counts matched N*(N-1)/2 exactly for all workloads.
- Release collision benchmark ran successfully.
- Benchmark results written to `results/collision_baseline.csv`.
- Existing gameplay (movement, shooting, enemy FSM, score, restart) verified functional.
- Sanitizer build: build-asan configured and compiled successfully with -DENABLE_SANITIZERS=ON.
  AABB tests passed under ASan/UBSan (CTest: 100% passed in build-asan).
  Full game runtime sanitizer exercise requires interactive display (not available in CI).

**Benchmark Results (Release build, seed 1337):**

| N    | Candidate Checks | Intersections | Median Time (us) | ns/Candidate |
|------|-----------------|---------------|------------------|--------------|
| 100  | 4,950           | 28            | 25               | 5.05         |
| 500  | 124,750         | 171           | 2,131            | 17.09        |
| 1000 | 499,500         | 325           | 6,936            | 13.89        |
| 2500 | 3,123,750       | 759           | 55,590           | 17.80        |
| 5000 | 12,497,500      | 1,637         | 169,939          | 13.60        |
