#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Utils.h"
#include <chrono>

#ifdef _WIN32
#include <process.h>
#else

#include <unistd.h>

#endif

std::ofstream PhaseTime::ofs("phase_time.log", std::ios::out);

[[noreturn]] void panic(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

int getPid() {
    return (int) getpid();
}

PhaseTime::PhaseTime(const char *name)
        : start(std::chrono::system_clock::now()), name(name) {
}

PhaseTime::~PhaseTime() {
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    ofs << "[PhaseTime] " << name << " ( " << duration.count() / 1000000.0 << "s(" << duration.count() << ") )\n";
    ofs.flush();
}

void PhaseTime::reset() {
    start = std::chrono::system_clock::now();
}
