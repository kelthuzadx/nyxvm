cmake_minimum_required(VERSION 3.5)
project(nyx)

set(CMAKE_CXX_STANDARD 17)
set(NYX_SRC nyx/Main.cpp nyx/Parser.cpp nyx/Utils.cpp nyx/Token.h nyx/AstVisitor.h nyx/AstDump.cpp nyx/AstDump.h)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build)

# Nyx compiler
add_executable(nyx ${NYX_SRC})