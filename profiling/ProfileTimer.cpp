#include "ProfileTimer.h"

#include <thread>

ProfileTimer::ProfileTimer(const std::string &name) {
    using namespace std::chrono;

    result.name = name;
    static long long lastStartTime = 0;
    result.start = time_point_cast<microseconds>(high_resolution_clock::now()).time_since_epoch().count();

    result.start += (result.start == lastStartTime ? 2 : 0);
    lastStartTime = result.start;
}

ProfileTimer::~ProfileTimer() {
    stop();
}

void ProfileTimer::stop() {
    using namespace std::chrono;

    const auto elapsedTimePoint = high_resolution_clock::now();
    result.end = time_point_cast<microseconds>(elapsedTimePoint).time_since_epoch().count();
    result.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());

    Profiler::Instance().addProfile(result);
}
