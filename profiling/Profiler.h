#pragma once

#include <fstream>
#include <mutex>

struct ProfileResult {
    std::string name = "Default";
    long long start = 0;
    long long end = 0;
    size_t threadId = 0;
};

class Profiler {
    std::string outputFile = "profile.json";
    std::ofstream fos;
    std::mutex lock;
    int profileCount = 0;

    Profiler();
    void writeHeader();
    void writeFooter();

public:
    static Profiler& Instance();
    ~Profiler();

    void addProfile(const ProfileResult& result);
};