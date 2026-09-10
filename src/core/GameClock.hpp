#pragma once

#include <chrono>

class GameClock {
public:
    GameClock();

    void tick();

    float deltaTime() const;
    double elapsedTime() const;

private:
    using Clock = std::chrono::steady_clock;

    Clock::time_point startTime_;
    Clock::time_point previousTime_;

    float deltaTime_{0.0f};
    double elapsedTime_{0.0};
};
