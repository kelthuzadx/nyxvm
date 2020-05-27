cmake_minimum_required(VERSION 3.5)
project(nyx)

set(CMAKE_CXX_STANDARD 17)
set(NYX_SRC nyx/Main.cpp nyx/parser/Parser.cpp nyx/util/Utils.cpp nyx/parser/AstDump.cpp nyx/parser/Ast.cpp nyx/bytecode/BytecodeGenerator.cpp nyx/bytecode/BytecodeGenerator.h nyx/bytecode/Opcode.h nyx/bytecode/Bytecode.cpp nyx/bytecode/Bytecode.h nyx/runtime/Global.h nyx/interpreter/Interpreter.cpp nyx/interpreter/Interpreter.h nyx/runtime/Builtin.cpp nyx/object/NObject.cpp nyx/object/NObject.h nyx/runtime/NyxVM.cpp nyx/runtime/NyxVM.h nyx/runtime/Builtin.h nyx/interpreter/Frame.cpp nyx/interpreter/Frame.h nyx/bytecode/BytecodeDump.cpp nyx/bytecode/BytecodeDump.h nyx/bytecode/Opcode.cpp nyx/gc/GenHeap.cpp nyx/gc/GenHeap.h nyx/object/NType.cpp nyx/object/NType.h nyx/object/NHeader.cpp nyx/object/NHeader.h nyx/gc/Space.cpp nyx/gc/Space.h)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build)

# Nyx compiler
add_executable(nyx ${NYX_SRC})