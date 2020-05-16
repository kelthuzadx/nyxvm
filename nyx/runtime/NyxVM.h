#ifndef NYX_NYXVM_H
#define NYX_NYXVM_H

#include <unordered_map>
#include <string>

class NyxVM {
private:
    static const char *((builtin[])[2]);
public:
    static const char *findBuiltin(const std::string &name);

public:
    NyxVM() = default;

    void setup();

    void ignition(const char *script);
};


#endif //NYX_NYXVM_H
