#ifndef NYX_BUILTIN_H
#define NYX_BUILTIN_H

struct NValue;

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device
//
// Parameter:
//      arg1,arg2,...
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NValue* nyxffi_print(int argc, NValue** argv);

//===----------------------------------------------------------------------===//
// Print arbitrary arguments to standard output device with a line break char
//
// Parameter:
//      arg1,arg2,...
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NValue* nyxffi_println(int argc, NValue** argv);

//===----------------------------------------------------------------------===//
// Internal string representation of given object's type
//
// Parameter:
//      arg1 arbitrary object
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NValue* nyxffi_typeof(int argc, NValue** argv);

//===----------------------------------------------------------------------===//
// length of the array or string
//
// Parameter:
//      arg1 array or string
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NValue* nyxffi_len(int argc, NValue** argv);

//===----------------------------------------------------------------------===//
// exit the program
// Parameter:
//      arg1 exit code
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NValue* nyxffi_exit(int argc, NValue** argv);

//===----------------------------------------------------------------------===//
// standard assertion
// Parameter:
//      arg1 condition
//      arg2 [optional] failure message
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NValue* nyxffi_assert(int argc, NValue** argv);

//===----------------------------------------------------------------------===//
// produce [a,b) array
//
// Parameter:
//      arg1 start range,inclusively
//      arg2 end range,exclusively
// Return:
//      none
//===----------------------------------------------------------------------===//
extern "C" NValue* nyxffi_range(int argc, NValue** argv);
#endif // NYX_BUILTIN_H
