#ifndef NYX_UTILS_H
#define NYX_UTILS_H

#include <any>
#include <cassert>
#include <chrono>
#include <deque>
#include <string>

template <typename _DesireType, typename... _ArgumentType>
inline bool anyone(_DesireType k, _ArgumentType... args) {
    return ((args == k) || ...);
}

[[noreturn]] void panic(const char* format, ...);

int getPid();

class PhaseTime {
  private:
    static std::ofstream ofs;
    std::chrono::time_point<std::chrono::system_clock> start;
    const char* name;

  public:
    explicit PhaseTime(const char* name);

    ~PhaseTime();

    void reset();
};

#endif