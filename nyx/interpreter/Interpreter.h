#ifndef NYX_INTERPRETER_H
#define NYX_INTERPRETER_H

#include "../runtime/Global.hpp"
#include "../bytecode/MetaArea.h"

class Interpreter {
private:
    const nyx::int8 *bytecodes;
    MetaArea* meta{};
    int bci;

public:
    Interpreter(MetaArea* meta);

    void execute();
};


#endif //NYX_INTERPRETER_H
