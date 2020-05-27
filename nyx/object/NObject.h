#ifndef NYX_NOBJECT_H
#define NYX_NOBJECT_H

#include <utility>

#include "../runtime/Global.h"
#include "NHeader.h"
#include "NType.h"

//===----------------------------------------------------------------------===//
// Description of object. The only way to create an NObject is to call heap
// allocator and cast type
//===----------------------------------------------------------------------===//
struct Bytecode;

template<typename AsType>
inline AsType* as(void *addr){ return static_cast<AsType*>(addr);}

struct NonInstantiable{
    void* operator new(size_t size)= delete;
    void operator delete(void* ptr)= delete;
    explicit NonInstantiable()= delete;
    ~NonInstantiable()= delete;
};

class NBase:public NonInstantiable {
  private:
    NHeader* header;

  public:
    NHeader* getHeader(){ return header;}
};

class NInt:public NBase{
  private:
    int32 val;

  public:
    static int32 size(){ return sizeof(NInt); }

    void setValue(int32 val){ this->val = val;}
};

class NDouble:public NBase{
  private:
    int64 val;

  private:
    void setValue(double val){ this->val = (int64)(val);}
};

class NChar:public NBase{
  private:
    int8 val;

  private:
    void setValue(int8 val){ this->val = val;}
};

class NObject :public NBase{
  private:
    NType* type;
  public:

};
//
//struct NString : public NObject {
//    explicit NString(std::string value);
//
//    std::string value;
//
//    std::string toString() override;
//};
//
//struct NArray : public NObject {
//    explicit NArray(int length);
//
//    ~NArray() override;
//
//    std::string toString() override;
//
//    int length;
//    NObject** array;
//};
//
//struct NCallable : public NObject {
//    explicit NCallable(Bytecode* code, bool isNative);
//
//    explicit NCallable(const char* code, bool isNative);
//
//    ~NCallable() override;
//
//    std::string toString() override;
//
//    bool isNative;
//
//    union {
//        Bytecode* bytecode;
//        const char* native;
//    } code{};
//};

#endif // NYX_NOBJECT_H
