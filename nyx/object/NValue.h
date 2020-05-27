#ifndef NYX_NVALUE_H
#define NYX_NVALUE_H

#include <utility>

#include "../runtime/Global.h"
#include "NonInstantiable.h"
#include "NHeader.h"
#include "NType.h"

//===----------------------------------------------------------------------===//
// Description of object. The only way to create an NValue is to call heap
// allocator and cast type.
//
// NEVER ADD VIRTUAL TO OBJECT MODEL
//===----------------------------------------------------------------------===//
struct Bytecode;

template<typename AsType>
inline AsType* as(void *addr){ return static_cast<AsType*>(addr);}

template<typename IsType>
inline bool is(void * addr){
    return as<IsType*>(addr)->getHeader()->isType<IsType*>();
}

class NValue :public NonInstantiable {
  private:
    NHeader* header;

  public:
    NHeader* getHeader(){ return header;}
};

class NInt:public NValue {
  private:
    int32 val;

  public:
    static int32 size(){ return sizeof(NInt); }

    void setValue(int32 val){ this->val = val;}
};

class NDouble:public NValue {
  private:
    int64 val;

  public:
    static int32 size(){ return sizeof(NDouble); }
    void setValue(double val){ this->val = (int64)(val);}
};

class NChar:public NValue {
  private:
    int8 val;

  public:
    static int32 size(){ return sizeof(NChar); }

    void setValue(int8 val){ this->val = val;}
};

class NObject :public NValue {
  private:
    NType* type;
  public:
    static int32 size(){ return -1; }
};
#endif // NYX_NVALUE_H
