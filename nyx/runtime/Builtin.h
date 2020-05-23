#ifndef NYX_BUILTIN_H
#define NYX_BUILTIN_H

struct NObject;

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device
//
// Parameter:
//      arg1,arg2,...
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NObject* nyxffi_print(int argc, NObject** argv);

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device with a line break char
//
// Parameter:
//      arg1,arg2,...
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NObject* nyxffi_println(int argc, NObject** argv);

//===----------------------------------------------------------------------===//
// Internal string representation of given object's type
//
// Parameter:
//      arg1 arbitrary object
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NObject* nyxffi_typeof(int argc, NObject** argv);

//===----------------------------------------------------------------------===//
// length of the array or string
//
// Parameter:
//      arg1 array or string
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NObject* nyxffi_len(int argc, NObject** argv);

//===----------------------------------------------------------------------===//
// exit the program
// Parameter:
//      arg1 exit code
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NObject* nyxffi_exit(int argc, NObject** argv);

//===----------------------------------------------------------------------===//
// standard assertion
// Parameter:
//      arg1 condition
//      arg2 [optional] failure message
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NObject* nyxffi_assert(int argc, NObject** argv);

//===----------------------------------------------------------------------===//
// produce [a,b) array
//
// Parameter:
//      arg1 start range,inclusively
//      arg2 end range,exclusively
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NObject* nyxffi_range(int argc, NObject** argv);
#endif // NYX_BUILTIN_H
