#ifndef NYX_BUILTIN_H
#define NYX_BUILTIN_H

struct Object;

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device
//
// Parameter:
//      arg1,arg2,...
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" Object* nyxffi_print(int argc, Object** argv);

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device with a line break char
//
// Parameter:
//      arg1,arg2,...
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" Object* nyxffi_println(int argc, Object** argv);

//===----------------------------------------------------------------------===//
// Internal string representation of given object's type
//
// Parameter:
//      arg1 arbitrary object
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" Object* nyxffi_typeof(int argc, Object** argv);

//===----------------------------------------------------------------------===//
// length of the array or string
//
// Parameter:
//      arg1 array or string
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" Object* nyxffi_len(int argc, Object** argv);

//===----------------------------------------------------------------------===//
// exit the program
// Parameter:
//      arg1 exit code
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" Object* nyxffi_exit(int argc, Object** argv);

//===----------------------------------------------------------------------===//
// standard assertion
// Parameter:
//      arg1 condition
//      arg2 [optional] failure message
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" Object* nyxffi_assert(int argc, Object** argv);
#endif // NYX_BUILTIN_H
