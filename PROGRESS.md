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

**Status**: Implementation Complete — Interactive Verification Pending

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
- Application launched successfully (pending human verification).
- WASD/camera behavior remains functional (pending human verification).
- Projectile shooting verified (pending human verification).
- Enemy spawning/FSM behavior verified (pending human verification).
- Enemy attacks/player health verified (pending human verification).
- Projectile/enemy hits and scoring verified (pending human verification).
- Game-over/restart behavior verified (pending human verification).
- Resize/minimize handling verified (pending human verification).
- Sanitizer run could not complete due to persistent CMake/FetchContent network hangs during dependency download.
