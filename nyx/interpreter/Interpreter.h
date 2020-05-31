#ifndef NYX_INTERPRETER_H
#define NYX_INTERPRETER_H

#include "../bytecode/Bytecode.h"
#include "../bytecode/Opcode.h"
#include "../gc/GenHeap.h"
#include "../object/NValue.h"
#include "../runtime/Global.h"
#include "Frame.h"

//===----------------------------------------------------------------------===//
// Simple and low performance C++ bytecode interpreter
//===----------------------------------------------------------------------===//
class Interpreter {
  private:
    std::vector<Frame*> stack;
    Frame* frame{};

  private:
    void neg(NValue* object);

    void createFrame(Bytecode* bytecode, int argc, NValue** argv);

    void destroyFrame(Bytecode* bytecode, bool hasReturnValue);

    void call(Bytecode* bytecodes, int bci);

    void loadFreeVar(FreeVar* freeVar);

    void storeFreeVar(FreeVar* freeVar, NValue* object);

    void execute(Bytecode* bytecode, int argc, NValue** argv);

  public:
    explicit Interpreter();

    ~Interpreter();

    void execute(Bytecode* bytecode);
};
#endif // NYX_INTERPRETER_H
