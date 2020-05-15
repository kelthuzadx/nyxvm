#ifndef NYX_INTERPRETER_H
#define NYX_INTERPRETER_H

#include "../runtime/Global.hpp"
#include "../bytecode/MetaArea.h"
#include "Object.h"

struct Frame {
    std::vector<Object *> slots;
};

class Interpreter {
private:
    MetaArea *meta{};
    std::vector<Frame *> stack;
    Frame *frame;

public:
    Interpreter(MetaArea *meta);

    void execute();
};


#endif //NYX_INTERPRETER_H
