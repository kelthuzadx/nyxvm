#ifndef NYX_NYXVM_H
#define NYX_NYXVM_H

#include <string>
#include <unordered_map>

class NyxVM {
  private:
    static const char*((builtin[])[2]);

  public:
    static const char* findBuiltin(const std::string& name);

  public:
    NyxVM() = default;

    static void setup();

    void ignition(const char* script);
};

#endif // NYX_NYXVM_H
