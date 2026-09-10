#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string_view>
#include <vector>

enum class ProfileSection : std::size_t {
    Frame,
    Input,
    GameUpdate,
    PlayerUpdate,
    ProjectileUpdate,
    EnemyAI,
    Collision,
    RenderSubmission,
    Count
};

std::string_view profileSectionName(ProfileSection section);

struct SectionStats {
    std::uint64_t count{0};
    double meanUs{0.0};
    double medianUs{0.0};
    double p95Us{0.0};
    double minUs{0.0};
    double maxUs{0.0};
};

using ProfileSummary = std::array<SectionStats, static_cast<std::size_t>(ProfileSection::Count)>;
using SectionArray = std::array<double, static_cast<std::size_t>(ProfileSection::Count)>;

struct FrameProfileSample {
    SectionArray microseconds{};
};

class Profiler {
public:
    void beginCapture(std::size_t expectedFrames);
    void endCapture();
    void clear();

    void beginFrame();
    void endFrame();

    void record(ProfileSection section, std::chrono::nanoseconds duration);

    bool capturing() const;

    ProfileSummary summarize() const;

    void writeSummaryCsv(const std::filesystem::path& path) const;
    void writeFrameCsv(const std::filesystem::path& path) const;

private:
    bool capturing_{false};
    std::vector<FrameProfileSample> samples_;
    FrameProfileSample currentFrame_;
};

class ScopedTimer {
public:
    ScopedTimer(Profiler& profiler, ProfileSection section);
    ~ScopedTimer();

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    Profiler& profiler_;
    ProfileSection section_;
    std::chrono::steady_clock::time_point start_;
};

#if VA_ENABLE_PROFILING
    #define VA_PROFILE_SCOPE(profiler, section) ScopedTimer timer_##__LINE__((profiler), (section))
#else
    #define VA_PROFILE_SCOPE(profiler, section)
#endif
