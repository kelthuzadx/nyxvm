#pragma once

#include <any>
#include <deque>
#include <string>

template<typename _DesireType, typename... _ArgumentType>
inline bool anyone(_DesireType k, _ArgumentType... args) {
    return ((args == k) || ...);
}

[[noreturn]] void panic(const char *format, ...);

int getPid();

class PhaseTime {
private:
    long start;
    const char *name;

public:
    explicit PhaseTime(const char *name);

    ~PhaseTime();

    void reset();
};

