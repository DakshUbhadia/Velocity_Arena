#include "RuntimeOptions.hpp"
#include <iostream>
#include <string_view>

RuntimeOptions parseRuntimeOptions(int argc, char** argv) {
    RuntimeOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        
        if (arg == "--help") {
            options.showHelp = true;
        } else if (arg == "--profile") {
            options.profile = true;
        } else if (arg == "--no-vsync") {
            options.vsync = false;
        } else if (arg == "--stress" && i + 1 < argc) {
            options.stressCount = std::stoull(argv[++i]);
        } else if (arg == "--collision" && i + 1 < argc) {
            std::string_view mode = argv[++i];
            if (mode == "grid") {
                options.collisionMode = GameplayCollisionMode::SpatialGrid;
            } else {
                options.collisionMode = GameplayCollisionMode::BruteForce;
            }
        } else if (arg == "--warmup-frames" && i + 1 < argc) {
            options.warmupFrames = std::stoull(argv[++i]);
        } else if (arg == "--measure-frames" && i + 1 < argc) {
            options.measureFrames = std::stoull(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            options.seed = std::stoul(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputPath = argv[++i];
        } else {
            std::cerr << "Unknown or malformed argument: " << arg << "\n";
        }
    }
    
    return options;
}
