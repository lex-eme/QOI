#include "Profiler.h"

Profiler::Profiler() {
    fos = std::ofstream(outputFile);
    writeHeader();
}

Profiler::~Profiler() {
    writeFooter();
}

Profiler& Profiler::Instance() {
    static Profiler instance;
    return instance;
}

void Profiler::addProfile(const ProfileResult &result) {
    std::lock_guard l(lock);

    if (profileCount++ > 0) {
        fos << ",";
    }
 
    std::string name = result.name;
    std::replace(name.begin(), name.end(), '"', '\'');
 
    fos << "{";
    fos << R"("cat":"function",)";
    fos << "\"dur\":" << (result.end - result.start) << ',';
    fos << R"("name":")" << name << "\",";
    fos << R"("ph":"X",)";
    fos << "\"pid\":0,";
    fos << "\"tid\":" << result.threadId << ",";
    fos << "\"ts\":" << result.start;
    fos << "}";
}

void Profiler::writeHeader() {
    fos << R"({"otherData": {},"traceEvents":[)";
}

void Profiler::writeFooter() {
    fos << "]}";
}

