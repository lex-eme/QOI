#pragma once

#include <chrono>

#include "Profiler.h"

#define PROFILING 1
#ifdef PROFILING
    #define PROFILE_SCOPE(name) ProfileTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION()  PROFILE_SCOPE(__FUNCTION__)
#else
    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
#endif

class ProfileTimer {
    ProfileResult result;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;

    void stop();

public:
    explicit ProfileTimer(const std::string& name);
    ~ProfileTimer();
};