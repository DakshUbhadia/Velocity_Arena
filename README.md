# Velocity Arena

A 3D arena shooter implemented in C++20 using OpenGL.

## Controls

- `W/A/S/D`     Move
- `Space`       Fire
- `R`           Restart after Game Over
- `Escape`      Quit

## Gameplay

Velocity Arena is a fast-paced survival arena shooter. Move around the arena to dodge chasing enemies while returning fire. Survive as long as you can and achieve the highest score!

## Performance Engineering

This engine uses an optimized broad-phase collision detection system:
- **Brute-force AABB Baseline**: A deterministic, reproducible O(N^2) reference implementation.
- **Uniform Spatial Grid Optimization**: A sparse XZ spatial hash grid reducing candidate pairs significantly.
- **Benchmark Methodology**: Same-run timings on identical reproducible setups with untimed cache warm-up passes.

### Collision Benchmark Results (N=5000)

| N | Baseline Candidates | Grid Candidates | Reduction | Baseline Time | Grid Time | Speedup |
|---|--------------------:|----------------:|----------:|--------------:|----------:|--------:|
| 5000 |         12,497,500 |           6,619 |     99.9% |      69,514us |   2,780us |  25.00x |

For full methodology and results across all entity counts and cell sizes, see `docs/performance.md`.

## Real-Time Performance Profiling

Velocity Arena includes a built-in CPU profiler to measure the impact of collision and rendering subsystems on the real frame time.

Example commands:

\\ash
./velocity_arena --profile
\
\\ash
./velocity_arena     --stress 1000     --collision brute     --warmup-frames 180     --measure-frames 600
\
\\ash
./velocity_arena     --stress 1000     --collision grid     --warmup-frames 180     --measure-frames 600
\EOF
