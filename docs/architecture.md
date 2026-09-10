# Velocity Arena Architecture

## Directory Structure
- `src/core/`: Contains core engine systems like `GameClock` for delta time and `Input` for keyboard handling.
- `src/graphics/`: Contains rendering abstractions like `Shader`, `Mesh`, `Renderer`, and `Camera`.
- `src/game/`: Contains gameplay-specific entities like `Player`.

## Core Components
- **GameClock**: Uses `std::chrono::steady_clock` to calculate the elapsed delta time between frames. It bounds the maximum delta time to avoid large jumps if the window pauses.
- **Input**: A lightweight wrapper around `glfwGetKey` holding a non-owning pointer to the `GLFWwindow`.

## Graphics Components
- **Camera**: Reusable view matrix manager taking a `position`, `target`, and `up` vector to construct `glm::lookAt()`. It is currently driven by the game loop to follow the player.
- **Renderer**: Handles the actual drawing operations (e.g., `glDrawArrays`) using provided `Mesh` and `Shader` combinations.

## Game Components
- **Game**: The central orchestration class. It owns `Player`, a `std::vector<Enemy>`, and a `std::vector<Projectile>`. It manages score, state, entity spawning, collisions, and gameplay loop. Main does not individually own these gameplay entities.
- **Player**: Represents the gameplay state of the player (health, position, facing direction).
- **Enemy**: Uses a finite-state machine (Idle, Chase, Attack) based on squared distance from the player.
- **Projectile**: A short-lived, linear-moving entity spawned by the player.

## Game Ownership Model

```
Application / main
        |
        +---- Renderer
        +---- Camera
        +---- Input
        +---- GameClock
        |
        +---- Game
                |
                +---- Player
                |
                +---- vector<Enemy>
                |
                +---- vector<Projectile>
```

**Why Game does not own Renderer**:
`Game` is strictly for gameplay logic and state. The `Renderer` requires OpenGL contexts and handles visual representation. Keeping them separated ensures that game state can be updated independently of how it is drawn, and `main.cpp` orchestrates the flow.

## Update Flow

```
Input
  |
  v
Game::update(dt)
  |
  +--> Player update
  |
  +--> Enemy FSM
  |
  +--> Projectile update
  |
  +--> Gameplay collision
  |
  +--> Score / Health / GameState
  |
  v
main.cpp
  |
  v
Renderer
```

## Lifetime and Ownership
- `main.cpp` manages the lifetime of OpenGL/GLFW contexts and resource creation.
- `Input` does not own the GLFW window, it only references it.
- GPU resources (Shaders, Meshes) are intentionally destroyed before `glfwTerminate()`.
