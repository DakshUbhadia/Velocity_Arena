## What We Built
Bootstrapped the project structure, CMake build system, and verified that a basic OpenGL context and window can be opened using GLFW and GLAD.

## Important Files
- `CMakeLists.txt`: Manages dependencies and build configuration.
- `src/main.cpp`: Entry point that initializes GLFW, creates a window, and runs a basic loop.

## Execution Flow
1. Initialize GLFW.
2. Create OpenGL 3.3 Core Profile context and window.
3. Load OpenGL functions using GLAD.
4. Run infinite loop: process input, clear screen, swap buffers.
5. Exit loop on Escape key and clean up GLFW.

## Important C++ Concepts
- `std::cout`, `std::cerr` for simple logging.

## Algorithms / Data Structures
- N/A

## 3D Math Used
- N/A

## Complexity
- N/A

## Memory/Lifetime Considerations
- GLFW requires manual cleanup via `glfwTerminate()`. This ensures system resources (like window handles) are released.

## Performance Considerations
- The game loop currently runs as fast as possible without a frame-time cap. We will need a `GameClock` and delta-time.

## Why This Design Was Chosen
- FetchContent in CMake makes dependencies reproducible without globally installing libraries on the host system (except core graphics drivers).
- GLAD + GLFW + GLM is the standard, well-documented lightweight stack for modern C++ OpenGL development.

## Alternatives
- Could have used SDL2 instead of GLFW, but GLFW is more specialized for OpenGL window creation and perfectly suited for this specification.

## Common Bugs
- Black screen or crash if GLAD is not initialized after `glfwMakeContextCurrent()`.

## Interview Questions
- Why do we need GLAD/GLEW? (To query the graphics driver for OS-specific function pointers for the OpenGL API at runtime).

## Things I Should Be Able To Explain Without Looking At Code
- The difference between `CMakeLists.txt` and a build system like `Make` or `Ninja`.
- The sequence of initializing a graphics window (library init -> window create -> context current -> loader init).
