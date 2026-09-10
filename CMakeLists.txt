cmake_minimum_required(VERSION 3.20)
project(VelocityArena VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(ENABLE_SANITIZERS "Enable sanitizers in Debug build" OFF)
option(ENABLE_PROFILING "Enable internal profiling" ON)
option(BUILD_TESTS "Build Catch2 unit tests" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)

# Dependencies via FetchContent
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

# nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(json)

# httplib
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.16.0
)
FetchContent_MakeAvailable(httplib)

# Catch2 (Optional)
if(BUILD_TESTS)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.3
    )
    FetchContent_MakeAvailable(Catch2)
endif()

add_executable(velocity_arena 
    src/main.cpp 
    src/external/src/gl.c
    src/core/GameClock.cpp
    src/core/Input.cpp
    src/game/Player.cpp
    src/game/Enemy.cpp
    src/game/Projectile.cpp
    src/game/Game.cpp
    src/graphics/Camera.cpp
    src/graphics/Shader.cpp
    src/graphics/Mesh.cpp
    src/graphics/Renderer.cpp
)

# Define directories
target_include_directories(velocity_arena PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/external/include
)

# Link libraries
target_link_libraries(velocity_arena PRIVATE
    glfw
    glm::glm
    nlohmann_json::nlohmann_json
    httplib::httplib
)

# Copy shaders post-build
add_custom_command(TARGET velocity_arena POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:velocity_arena>/shaders
)

if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(velocity_arena PRIVATE -fsanitize=address,undefined -g -O0 -Wall -Wextra -Wpedantic)
        target_link_options(velocity_arena PRIVATE -fsanitize=address,undefined)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(velocity_arena PRIVATE -Wall -Wextra -Wpedantic)
endif()
