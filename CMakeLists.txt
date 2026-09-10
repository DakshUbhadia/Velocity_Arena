cmake_minimum_required(VERSION 3.20)
project(VelocityArena VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_SANITIZERS  "Enable ASan/UBSan in Debug builds"         OFF)
option(ENABLE_PROFILING   "Enable internal profiling"                  ON)
option(ENABLE_NETWORKING  "Enable networking/REST components"          OFF)
option(BUILD_TESTS        "Build dependency-free AABB unit tests"      OFF)
option(BUILD_BENCHMARKS   "Build standalone collision benchmark"       OFF)

function(enable_project_sanitizers target_name)
    if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined -g -O0 -fno-omit-frame-pointer)
            
            get_target_property(target_type ${target_name} TYPE)
            if(NOT target_type STREQUAL "STATIC_LIBRARY")
                target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
            endif()
        endif()
    endif()
endfunction()

# Shared FetchContent dependency directory.
# Reuses downloaded sources across build/, build-release/, build-asan/, etc.
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.deps"
    CACHE PATH "Shared FetchContent cache directory" FORCE)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND  OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

# Networking dependencies -- only fetched when ENABLE_NETWORKING=ON.
# nlohmann/json and cpp-httplib will be added during the REST milestone.
if(ENABLE_NETWORKING)
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(json)
    FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.16.0
    )
    FetchContent_MakeAvailable(httplib)
endif()

# velocity_physics -- reusable AABB collision static library.
# Both velocity_arena and collision_benchmark link against this.
add_library(velocity_physics STATIC
    src/physics/AABB.cpp
    src/physics/Collision.cpp
    src/physics/SpatialGrid.cpp
)
target_include_directories(velocity_physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(velocity_physics PUBLIC glm::glm)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(velocity_physics PRIVATE -Wall -Wextra -Wpedantic)
endif()
enable_project_sanitizers(velocity_physics)

# velocity_arena -- main game executable
add_executable(velocity_arena
    src/main.cpp
    src/external/src/gl.c
    src/core/GameClock.cpp
    src/core/Input.cpp
    src/core/Profiler.cpp
    src/core/RuntimeOptions.cpp
    src/game/Player.cpp
    src/game/Enemy.cpp
    src/game/Projectile.cpp
    src/game/Game.cpp
    src/graphics/Camera.cpp
    src/graphics/Shader.cpp
    src/graphics/Mesh.cpp
    src/graphics/Renderer.cpp
)
target_include_directories(velocity_arena PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/external/include
)
target_link_libraries(velocity_arena PRIVATE
    velocity_physics
    glfw
    glm::glm
)
if(ENABLE_NETWORKING)
    target_link_libraries(velocity_arena PRIVATE
        nlohmann_json::nlohmann_json
        httplib::httplib
    )
endif()

add_custom_command(TARGET velocity_arena POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:velocity_arena>/shaders
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(velocity_arena PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(ENABLE_PROFILING)
    target_compile_definitions(velocity_arena PRIVATE VA_ENABLE_PROFILING=1)
else()
    target_compile_definitions(velocity_arena PRIVATE VA_ENABLE_PROFILING=0)
endif()

enable_project_sanitizers(velocity_arena)

# collision_benchmark -- only when BUILD_BENCHMARKS=ON
if(BUILD_BENCHMARKS)
    add_executable(collision_benchmark benchmarks/collision_benchmark.cpp)
    target_link_libraries(collision_benchmark PRIVATE velocity_physics)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(collision_benchmark PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    enable_project_sanitizers(collision_benchmark)
endif()

# aabb_tests -- only when BUILD_TESTS=ON. No external testing framework needed.
if(BUILD_TESTS)
    enable_testing()
    
    add_executable(aabb_tests tests/aabb_tests.cpp)
    target_link_libraries(aabb_tests PRIVATE velocity_physics)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(aabb_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    enable_project_sanitizers(aabb_tests)
    add_test(NAME AABBTests COMMAND aabb_tests)

    add_executable(spatial_grid_tests tests/spatial_grid_tests.cpp)
    target_link_libraries(spatial_grid_tests PRIVATE velocity_physics)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(spatial_grid_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    enable_project_sanitizers(spatial_grid_tests)
    add_test(NAME SpatialGridTests COMMAND spatial_grid_tests)

    add_executable(profiler_tests tests/profiler_tests.cpp src/core/Profiler.cpp)
    target_include_directories(profiler_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(profiler_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    enable_project_sanitizers(profiler_tests)
    add_test(NAME ProfilerTests COMMAND profiler_tests)
endif()
