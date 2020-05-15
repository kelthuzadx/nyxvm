cmake_minimum_required(VERSION 3.5)
project(nyx)

set(CMAKE_CXX_STANDARD 17)
set(NYX_SRC nyx/Main.cpp nyx/parser/Parser.cpp nyx/runtime/Utils.cpp nyx/parser/AstDump.cpp nyx/parser/Ast.cpp nyx/bytecode/BytecodeGenerator.cpp nyx/bytecode/BytecodeGenerator.h nyx/bytecode/Opcode.h nyx/bytecode/MetaArea.cpp nyx/bytecode/MetaArea.h nyx/runtime/Global.hpp nyx/interpreter/Interpreter.cpp nyx/interpreter/Interpreter.h)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build)

# Nyx compiler
add_executable(nyx ${NYX_SRC})