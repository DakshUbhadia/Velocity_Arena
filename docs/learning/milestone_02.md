# Milestone 2 Learnings

### Timing
- `std::chrono::steady_clock` is preferred for game loops because it is monotonic and isn't affected by system clock adjustments.
- `deltaTime` ensures the game runs at the same speed regardless of the frame rate.
- Capping `deltaTime` prevents giant movement leaps if the process is suspended or hangs.

### Ownership & Architecture
- `Input` only references the `GLFWwindow*` but does not destroy it. This prevents double-free and use-after-free bugs.
- `Player` owns gameplay state but does not call OpenGL functions directly. This separates game logic from rendering logic.

### 3D Movement
- Frame-rate independence requires `movement = direction * speed * deltaTime`.
- Normalizing the diagonal movement vector is essential to prevent players from moving 40% faster diagonally.
- Third-person follow cameras are achieved simply by applying a fixed positional offset to the player's position, and pointing the `target` at the player's position.
