#ifndef NYX_BUILTIN_H
#define NYX_BUILTIN_H

struct Object;

extern "C" void nyxffi_print(int argc, Object **argv);

extern "C" void nyxffi_println(int argc, Object **argv);


#endif //NYX_BUILTIN_H
