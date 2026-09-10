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
- **Player**: Represents the gameplay state of the player (position, movement speed). Uses a normalized movement vector scaled by delta time to move independently of frame rate. **Note**: `Player` does not handle rendering logic directly. It simply exposes its `modelMatrix()`.

## Lifetime and Ownership
- `main.cpp` manages the lifetime of OpenGL/GLFW contexts and resource creation.
- `Input` does not own the GLFW window, it only references it.
- GPU resources (Shaders, Meshes) are intentionally destroyed before `glfwTerminate()`.
