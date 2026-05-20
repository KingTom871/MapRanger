/**
 * @file Profiler.h
 * @brief Simple profiler for performance monitoring.
 */
#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>

class Profiler {
public:
    static Profiler& instance() {
        static Profiler profiler;
        return profiler;
    }

    void record(const char* name, double milliseconds) {
        if (!enabled) {
            return;
        }

        Stat& stat = stats[name];
        stat.totalMilliseconds += milliseconds;
        ++stat.samples;
    }

    void printIfDue() {
        if (!enabled) {
            return;
        }

        const auto now = Clock::now();
        if (now - lastPrint < std::chrono::seconds(5)) {
            return;
        }

        lastPrint = now;

        if (stats.empty()) {
            return;
        }

        std::cout << "[Perf]";
        for (const auto& [name, stat] : stats) {
            if (stat.samples == 0) {
                continue;
            }

            const double average = stat.totalMilliseconds / static_cast<double>(stat.samples);
            std::cout << ' ' << name << '=' << average << "ms";
        }
        std::cout << '\n';

        stats.clear();
    }

    void setEnabled(bool enabled) {
        this->enabled = enabled;
        if (!enabled) {
            stats.clear();
        }
    }

private:
    using Clock = std::chrono::steady_clock;

    struct Stat {
        double totalMilliseconds = 0.0;
        int samples = 0;
    };

    std::unordered_map<std::string, Stat> stats;
    Clock::time_point lastPrint = Clock::now();
    bool enabled = true;
};

class ScopedTimer {
public:
    explicit ScopedTimer(const char* name)
        : name(name), start(Clock::now()) {
    }

    ~ScopedTimer() {
        const auto end = Clock::now();
        const std::chrono::duration<double, std::milli> elapsed = end - start;
        Profiler::instance().record(name, elapsed.count());
    }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    using Clock = std::chrono::steady_clock;

    const char* name = nullptr;
    Clock::time_point start;
};
