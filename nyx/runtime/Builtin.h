#ifndef NYX_BUILTIN_H
#define NYX_BUILTIN_H

struct Object;

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device
//===----------------------------------------------------------------------===//
extern "C" void nyxffi_print(int argc, Object **argv);

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device with a line break char
//===----------------------------------------------------------------------===//
extern "C" void nyxffi_println(int argc, Object **argv);


#endif //NYX_BUILTIN_H
