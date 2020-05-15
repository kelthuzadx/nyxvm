#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "Utils.hpp"


[[noreturn]] void panic(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

PhaseTime::PhaseTime(const char *name) : name(name) {
    start = clock();
}

PhaseTime::~PhaseTime() {
    long end = clock();
    double duration = (end - start) / 1000.0;
    std::cout << "[PhaseTime] " << name << " ( " << duration << "s )\n";
}
