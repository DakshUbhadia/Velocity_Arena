# Milestone 1 — Basic 3D Rendering

## What We Built
We successfully built a foundational 3D rendering pipeline. It features abstract RAII-compliant classes to handle OpenGL shaders (`Shader`) and position-based vertex geometry (`Mesh`). A lightweight `Renderer` class utilizes these to orchestrate draw calls using Model, View, and Projection (MVP) transformations, rendering an environment with a floor and multiple cubes.

## Files Added
- `src/graphics/Shader.hpp` & `src/graphics/Shader.cpp`
- `src/graphics/Mesh.hpp` & `src/graphics/Mesh.cpp`
- `src/graphics/Renderer.hpp` & `src/graphics/Renderer.cpp`
- `src/graphics/Primitives.hpp`
- `shaders/basic.vert` & `shaders/basic.frag`
- `docs/mathematics.md`
- `docs/learning/milestone_01.md`

## Files Modified
- `CMakeLists.txt`
- `src/main.cpp`
- `PROGRESS.md`

## Execution Flow
1. GLFW initializes, creating a window and an OpenGL 3.3 Core profile context.
2. GLAD dynamically loads OpenGL functions for the active context.
3. In a scoped block, the `Renderer`, `Shader`, and two `Mesh`es (cube and floor) are constructed.
4. The main application loop polls input (handling Escape to close) and calculates projection and view matrices.
5. The depth and color buffers are cleared each frame.
6. MVP uniform matrices are configured in the shader and passed along with the static geometry through the `Renderer`.
7. Once the loop ends, the scope block exits, safely invoking destructors and deleting all GPU resources via RAII before `glfwTerminate()` executes.

## OpenGL Rendering Pipeline
The pipeline takes vertices passed from our C++ program (stored in a VBO), passes them to our GLSL vertex shader where they undergo matrix multiplication to transition to clip space, and maps them to normalized device coordinates. The rasterizer determines which pixels belong to the resulting triangles, triggering the fragment shader to color those pixels.

## Shader Compilation and Linking
GLSL shaders are parsed, compiled, and linked into a single shader program by the GPU driver at runtime. We use a helper utility within `Shader` that captures the compilation logs, verifying `GL_COMPILE_STATUS` and `GL_LINK_STATUS` to throw robust exceptions with informative terminal logs in case of a failure.

## VAO and VBO
The Vertex Array Object (VAO) stores the configuration of vertex data (attribute pointers). The Vertex Buffer Object (VBO) holds the actual vertex data floating point values in GPU memory. The `Mesh` class manages creating, binding, populating, and deleting these buffers automatically.

## Model Matrix
The Model matrix places an object correctly within the world. Multiple cubes are translated, rotated, and scaled individually using their distinct Model matrices.

## View Matrix
The View matrix shifts the world to simulate a static camera using `glm::lookAt()`.

## Projection Matrix
The Projection matrix distorts vertices in view-space to project a 3D perspective using `glm::perspective()` depending on the current window aspect ratio.

## Depth Testing
Depth testing stores distances in the Z-buffer, determining which rendered pixels are behind others to avoid rendering geometric layers in simply the order they were submitted to the GPU.

## Resource Ownership and RAII
Resource Acquisition Is Initialization (RAII) ensures that constructors allocate OpenGL resources (like shader program IDs or VBO handles) and destructors explicitly `glDelete...` them. By scoping these objects, they seamlessly clean up GPU footprint without memory leaks.

## Why GPU Resources Must Die Before the OpenGL Context
If `glfwTerminate()` happens first, the context is destroyed. Subsequent `glDelete...` calls from late destructors will either be ignored or cause crashes because the OpenGL instance that tracks those resources is already gone.

## Important C++ Concepts
- `std::span` usage for robust contiguous memory array arguments.
- `std::filesystem::path` for OS-agnostic file path operations.
- Deleting copy constructors (`= delete`) to avoid dangerous duplication of unique resource handlers.
- Scoped lifetime tracking ensuring deterministic cleanup without garbage collection.

## Important OpenGL Concepts
- Difference between `glGenBuffers`, `glBindBuffer`, and `glBufferData`.
- `glEnable(GL_DEPTH_TEST)` requiring matching `GL_DEPTH_BUFFER_BIT` clears.
- Shader lifecycle (`glCreateShader`, `glCompileShader`, `glAttachShader`, `glLinkProgram`).
- `glViewport` synchronization with resizing framebuffers.

## Common Bugs
- Black screen from multiplying MVP matrices in reverse order or misplacing the `glm::lookAt` camera.
- Triangles rendering improperly across overlaps due to missing depth buffer clearances.
- Crash on exit from improperly ordering GLFW termination relative to `Shader` or `Mesh` destructors.

## Design Decisions
- Chose not to introduce a `Camera` abstraction early, instead forcing reliance on a raw `glm::lookAt` setup in the `main.cpp` loop to directly visualize matrix mathematics.
- Disabled copy semantics for all Graphics resource abstractions.

## Alternatives We Did Not Use
- Index buffers (`EBO`), opting for the simplicity of raw triangle lists.
- Dynamically loaded assets or `MeshBuilder` hierarchies, employing hardcoded static geometries in `Primitives.hpp` to reduce clutter.

## Interview Questions

### OpenGL
1. **What is OpenGL?** A cross-platform standard API for rendering 2D and 3D graphics, executed directly on the GPU.
2. **What does GLFW do?** It manages OS-specific tasks like creating windows, listening to input, and initializing the OpenGL context.
3. **What does GLAD do?** It resolves OpenGL function pointers provided by the graphics driver for the current system runtime.
4. **What is a shader?** A small program executing directly on the GPU designed to process pipeline rendering stages.
5. **What is a vertex shader?** Computes the final transformed position (and metadata) of each vertex.
6. **What is a fragment shader?** Computes the output color of each individual rasterized pixel fragment.
7. **What is a shader program?** The linked, executable combination of vertex and fragment shaders.
8. **What happens during shader linking?** The shader stages are connected, verifying compatible variable outputs/inputs and resolving memory locations.
9. **What is a VBO?** Vertex Buffer Object, an array of raw vertex data bytes placed into GPU memory.
10. **What is a VAO?** Vertex Array Object, records bindings and layouts describing how VBO data feeds into the shader attributes.
11. **What does `glDrawArrays()` do?** Triggers a draw call pulling a designated amount of vertices from the currently active VAO layout.
12. **Why are we using `GL_TRIANGLES`?** Triangles are the base polygon unit supported by modern graphics hardware.
13. **What is depth testing?** An algorithm evaluating a fragment's distance from the camera to conditionally draw it if it's closer than the existing pixel.
14. **What happens if depth testing is disabled?** Triangles are painted precisely in submission order (painter's algorithm), making far objects occlude near ones incorrectly.

### Mathematics
15. **What is a model matrix?** Transforms local object-space coordinates to absolute world coordinates.
16. **What is a view matrix?** Transforms world coordinates to be positioned relative to the camera's location and direction.
17. **What is a projection matrix?** Maps a 3D view-space into normalized 2D device coordinates, simulating lens distortions like perspective.
18. **What does `glm::lookAt()` construct?** A view matrix, based on camera position, look target, and a defined 'up' vector.
19. **What does `glm::perspective()` construct?** A perspective projection matrix.
20. **What is field of view?** The vertical angular extent of the observable world seen at any given moment.
21. **Why does aspect ratio matter?** It scales the horizontal dimension to prevent stretched rendering on non-square framebuffers.
22. **Why do we need a near plane?** To cap calculation limits and prevent rendering vertices mathematically behind the camera.
23. **Why do we need a far plane?** To cap calculation limits and allocate precise z-buffer distribution.
24. **Why is the order `Projection * View * Model * position`?** Because matrix-vector math resolves right-to-left. 
25. **Why is a 3D position represented as a `vec4` with `w = 1`?** So that translation offsets from 4x4 matrices apply, and perspective projection retains scale data.

### C++
26. **Why does `Shader` delete its copy constructor?** To prevent multiple C++ instances claiming the exact same OpenGL program ID, which would trigger double-deletions on destruction.
27. **Why does `Mesh` delete its copy constructor?** For the exact same reason as `Shader`, but applying to GPU buffer IDs.
28. **What does RAII mean here?** Tying the acquisition of GPU allocations directly to C++ constructors, and releasing them securely within destructors upon going out of scope.
29. **Who owns the VBO?** The `Mesh` instance.
30. **Who owns the VAO?** The `Mesh` instance.
31. **Who owns the shader program?** The `Shader` instance.
32. **Why must those resources be deleted before the OpenGL context?** Because once GLFW terminates the context, the OpenGL subsystem is dismantled, leaving destructors executing invalid library calls.

## Things I Should Be Able To Explain Without Looking At Code
- The matrix transformation order and reason for right-to-left computation.
- Why a VAO must be bound before `glEnableVertexAttribArray` and `glVertexAttribPointer`.
- The necessity of tying OpenGL GPU lifetimes to C++ scopes before `glfwTerminate`.
