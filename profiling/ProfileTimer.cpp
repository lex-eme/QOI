#include "ProfileTimer.h"

#include <thread>

ProfileTimer::ProfileTimer(const std::string &name) {
    result.name = name;
    startTimePoint = std::chrono::high_resolution_clock::now();
}

ProfileTimer::~ProfileTimer() {
    stop();
}

void ProfileTimer::stop() {
    using namespace std::chrono;

    if (stopped) {
        return;
    }

    stopped = true;
    const auto elapsedTimePoint = high_resolution_clock::now();
    result.start = time_point_cast<microseconds>(startTimePoint).time_since_epoch().count();
    result.end = time_point_cast<microseconds>(elapsedTimePoint).time_since_epoch().count();
    result.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());

    Profiler::Instance().addProfile(result);
}
