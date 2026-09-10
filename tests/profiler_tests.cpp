#include <iostream>
#include <cassert>
#include <cmath>
#include "core/Profiler.hpp"

void testEmptyCapture() {
    Profiler p;
    p.beginCapture(10);
    p.endCapture();
    auto summary = p.summarize();
    assert(summary[static_cast<std::size_t>(ProfileSection::Frame)].count == 0);
    std::cout << "testEmptyCapture passed\n";
}

void testSingleSample() {
    Profiler p;
    p.beginCapture(10);
    p.beginFrame();
    p.record(ProfileSection::Frame, std::chrono::microseconds(100));
    p.endFrame();
    p.endCapture();
    
    auto summary = p.summarize();
    auto stats = summary[static_cast<std::size_t>(ProfileSection::Frame)];
    assert(stats.count == 1);
    assert(std::abs(stats.meanUs - 100.0) < 0.001);
    assert(std::abs(stats.medianUs - 100.0) < 0.001);
    assert(std::abs(stats.minUs - 100.0) < 0.001);
    assert(std::abs(stats.maxUs - 100.0) < 0.001);
    std::cout << "testSingleSample passed\n";
}

void testOddSampleSet() {
    Profiler p;
    p.beginCapture(10);
    for (int i = 1; i <= 5; ++i) {
        p.beginFrame();
        p.record(ProfileSection::Frame, std::chrono::microseconds(i * 10)); // 10, 20, 30, 40, 50
        p.endFrame();
    }
    p.endCapture();
    
    auto summary = p.summarize();
    auto stats = summary[static_cast<std::size_t>(ProfileSection::Frame)];
    assert(stats.count == 5);
    assert(std::abs(stats.medianUs - 30.0) < 0.001);
    assert(std::abs(stats.meanUs - 30.0) < 0.001);
    assert(std::abs(stats.p95Us - 50.0) < 0.001);
    std::cout << "testOddSampleSet passed\n";
}

void testEvenSampleSet() {
    Profiler p;
    p.beginCapture(10);
    for (int i = 1; i <= 6; ++i) {
        p.beginFrame();
        p.record(ProfileSection::Frame, std::chrono::microseconds(i * 10)); // 10, 20, 30, 40, 50, 60
        p.endFrame();
    }
    p.endCapture();
    
    auto summary = p.summarize();
    auto stats = summary[static_cast<std::size_t>(ProfileSection::Frame)];
    assert(stats.count == 6);
    assert(std::abs(stats.medianUs - 35.0) < 0.001); // (30+40)/2
    assert(std::abs(stats.meanUs - 35.0) < 0.001);
    std::cout << "testEvenSampleSet passed\n";
}

int main() {
    testEmptyCapture();
    testSingleSample();
    testOddSampleSet();
    testEvenSampleSet();
    std::cout << "All profiler tests passed!\n";
    return 0;
}
