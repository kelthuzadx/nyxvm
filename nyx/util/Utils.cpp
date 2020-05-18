#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Utils.h"

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

PhaseTime::PhaseTime(const char *name) : name(name) {
    start = clock();
}

PhaseTime::~PhaseTime() {
    long end = clock();
    double duration = (end - start) / 1000.0;
    ofs << "[PhaseTime] " << name << " ( " << duration << "s )\n";
    ofs.flush();
}
