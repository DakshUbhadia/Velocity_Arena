#include "core/GameClock.hpp"
#include <algorithm>

GameClock::GameClock() {
    startTime_ = Clock::now();
    previousTime_ = startTime_;
}

void GameClock::tick() {
    auto currentTime = Clock::now();
    
    std::chrono::duration<float> delta = currentTime - previousTime_;
    deltaTime_ = delta.count();
    
    // Clamp delta time to maximum of 0.1 seconds to avoid huge jumps
    // when dragging the window, pausing in debugger, etc.
    if (deltaTime_ > 0.1f) {
        deltaTime_ = 0.1f;
    }
    
    std::chrono::duration<double> elapsed = currentTime - startTime_;
    elapsedTime_ = elapsed.count();
    
    previousTime_ = currentTime;
}

float GameClock::deltaTime() const {
    return deltaTime_;
}

double GameClock::elapsedTime() const {
    return elapsedTime_;
}
