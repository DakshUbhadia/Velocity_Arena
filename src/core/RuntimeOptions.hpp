#pragma once

#include <cstdint>
#include <string>

enum class GameplayCollisionMode {
    BruteForce,
    SpatialGrid
};

struct RuntimeOptions {
    bool profile{false};
    std::size_t stressCount{0};
    GameplayCollisionMode collisionMode{GameplayCollisionMode::BruteForce};
    std::size_t warmupFrames{180};
    std::size_t measureFrames{600};
    std::uint32_t seed{1337};
    std::string outputPath{"results/runtime_profile"};
    bool vsync{true};
    bool showHelp{false};
};

RuntimeOptions parseRuntimeOptions(int argc, char** argv);
