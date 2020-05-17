#ifndef NYX_BUILTIN_H
#define NYX_BUILTIN_H

struct Object;

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device
//===----------------------------------------------------------------------===//
extern "C" Object *nyxffi_print(int argc, Object **argv);

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device with a line break char
//===----------------------------------------------------------------------===//
extern "C" Object *nyxffi_println(int argc, Object **argv);

//===----------------------------------------------------------------------===//
// Internal string representation of given object's type
//===----------------------------------------------------------------------===//
extern "C" Object *nyxffi_typeof(int argc, Object **argv);

//===----------------------------------------------------------------------===//
// length of the array or string
//===----------------------------------------------------------------------===//
extern "C" Object *nyxffi_len(int argc, Object **argv);
#endif //NYX_BUILTIN_H
