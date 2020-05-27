#ifndef NYX_NOBJECT_H
#define NYX_NOBJECT_H

#include <utility>

#include "../runtime/Global.h"
#include "NHeader.h"
#include "NType.h"

struct Bytecode;

//===----------------------------------------------------------------------===//
// Description of object. The only way to create an NObject is to call heap
// allocator and cast type
//===----------------------------------------------------------------------===//
class NObject{
  private:
    NHeader* header;
    NType* type;

  public:
    void* operator new(size_t size)= delete;
    void operator delete(void* ptr)= delete;
    explicit NObject()= delete;
    ~NObject()= delete;
  private:

};

class NInt{
  private:
    NHeader* header;
    int32 val;

  public:
    void* operator new(size_t size)= delete;
    void operator delete(void* ptr)= delete;
    explicit NInt()= delete;
    ~NInt()= delete;
  private:
  public:
    static int32 size(){ return sizeof(header)+sizeof(val); }
};

class NDouble{
  private:
    NHeader* header;
    int64 val;

  public:
    void* operator new(size_t size)= delete;
    void operator delete(void* ptr)= delete;
    explicit NDouble()= delete;
    ~NDouble()= delete;
  private:
};

class NChar{
  private:
    NHeader* header;
    int8 val;

  public:
    void* operator new(size_t size)= delete;
    void operator delete(void* ptr)= delete;
    explicit NChar()= delete;
    ~NChar()= delete;
  private:
};

struct NString : public NObject {
    explicit NString(std::string value);

    std::string value;

    std::string toString() override;
};

struct NArray : public NObject {
    explicit NArray(int length);

    ~NArray() override;

    std::string toString() override;

    int length;
    NObject** array;
};

struct NCallable : public NObject {
    explicit NCallable(Bytecode* code, bool isNative);

    explicit NCallable(const char* code, bool isNative);

    ~NCallable() override;

    std::string toString() override;

    bool isNative;

    union {
        Bytecode* bytecode;
        const char* native;
    } code{};
};

#endif // NYX_NOBJECT_H
