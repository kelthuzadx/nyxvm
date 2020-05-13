#include <cstdarg>
#include <cstdio>
#include <cstdlib>

[[noreturn]] void panic(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}