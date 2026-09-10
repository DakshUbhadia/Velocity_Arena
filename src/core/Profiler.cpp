#include "Profiler.hpp"
#include <algorithm>
#include <fstream>
#include <cmath>

std::string_view profileSectionName(ProfileSection section) {
    switch (section) {
        case ProfileSection::Frame: return "Frame";
        case ProfileSection::Input: return "Input";
        case ProfileSection::GameUpdate: return "GameUpdate";
        case ProfileSection::PlayerUpdate: return "PlayerUpdate";
        case ProfileSection::ProjectileUpdate: return "ProjectileUpdate";
        case ProfileSection::EnemyAI: return "EnemyAI";
        case ProfileSection::Collision: return "Collision";
        case ProfileSection::RenderSubmission: return "RenderSubmission";
        default: return "Unknown";
    }
}

void Profiler::beginCapture(std::size_t expectedFrames) {
    clear();
    samples_.reserve(expectedFrames);
    capturing_ = true;
}

void Profiler::endCapture() {
    capturing_ = false;
}

void Profiler::clear() {
    samples_.clear();
    capturing_ = false;
}

void Profiler::beginFrame() {
    if (!capturing_) return;
    currentFrame_ = FrameProfileSample{};
}

void Profiler::endFrame() {
    if (!capturing_) return;
    samples_.push_back(currentFrame_);
}

void Profiler::record(ProfileSection section, std::chrono::nanoseconds duration) {
    if (!capturing_) return;
    currentFrame_.microseconds[static_cast<std::size_t>(section)] += 
        std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(duration).count();
}

bool Profiler::capturing() const {
    return capturing_;
}

ProfileSummary Profiler::summarize() const {
    ProfileSummary summary{};
    if (samples_.empty()) return summary;

    std::size_t sectionCount = static_cast<std::size_t>(ProfileSection::Count);
    
    for (std::size_t s = 0; s < sectionCount; ++s) {
        std::vector<double> vals;
        vals.reserve(samples_.size());
        
        double sum = 0.0;
        double min_val = std::numeric_limits<double>::max();
        double max_val = std::numeric_limits<double>::lowest();
        
        for (const auto& sample : samples_) {
            double v = sample.microseconds[s];
            vals.push_back(v);
            sum += v;
            if (v < min_val) min_val = v;
            if (v > max_val) max_val = v;
        }
        
        std::sort(vals.begin(), vals.end());
        
        SectionStats& st = summary[s];
        st.count = vals.size();
        st.meanUs = sum / vals.size();
        st.minUs = min_val;
        st.maxUs = max_val;
        
        if (vals.size() % 2 == 1) {
            st.medianUs = vals[vals.size() / 2];
        } else {
            st.medianUs = (vals[vals.size() / 2 - 1] + vals[vals.size() / 2]) / 2.0;
        }
        
        std::size_t p95Index = static_cast<std::size_t>(std::ceil(vals.size() * 0.95)) - 1;
        if (p95Index >= vals.size()) p95Index = vals.size() - 1;
        st.p95Us = vals[p95Index];
    }
    
    return summary;
}

void Profiler::writeSummaryCsv(const std::filesystem::path& path) const {
    std::ofstream out(path);
    if (!out) return;
    
    out << "Section,Count,Mean(us),Median(us),P95(us),Min(us),Max(us)\n";
    auto summary = summarize();
    for (std::size_t s = 0; s < static_cast<std::size_t>(ProfileSection::Count); ++s) {
        const auto& st = summary[s];
        out << profileSectionName(static_cast<ProfileSection>(s)) << ","
            << st.count << ","
            << st.meanUs << ","
            << st.medianUs << ","
            << st.p95Us << ","
            << st.minUs << ","
            << st.maxUs << "\n";
    }
}

void Profiler::writeFrameCsv(const std::filesystem::path& path) const {
    std::ofstream out(path);
    if (!out) return;
    
    out << "Frame";
    for (std::size_t s = 0; s < static_cast<std::size_t>(ProfileSection::Count); ++s) {
        out << "," << profileSectionName(static_cast<ProfileSection>(s)) << "(us)";
    }
    out << "\n";
    
    for (std::size_t i = 0; i < samples_.size(); ++i) {
        out << i;
        for (std::size_t s = 0; s < static_cast<std::size_t>(ProfileSection::Count); ++s) {
            out << "," << samples_[i].microseconds[s];
        }
        out << "\n";
    }
}

ScopedTimer::ScopedTimer(Profiler& profiler, ProfileSection section)
    : profiler_(profiler), section_(section), start_(std::chrono::steady_clock::now()) {
}

ScopedTimer::~ScopedTimer() {
    auto now = std::chrono::steady_clock::now();
    profiler_.record(section_, now - start_);
}
