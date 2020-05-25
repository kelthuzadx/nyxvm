#ifndef NYX_NYXVM_H
#define NYX_NYXVM_H

#include <string>
#include <unordered_map>

class NyxVM {
  public:
    NyxVM() = default;

    static void setup();

    void ignition(const char* script);
};

#endif // NYX_NYXVM_H
