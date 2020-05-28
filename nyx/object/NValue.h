#ifndef NYX_NVALUE_H
#define NYX_NVALUE_H

#include <utility>

#include "../runtime/Global.h"
#include "NHeader.h"
#include "NType.h"
#include "NonInstantiable.h"

//===----------------------------------------------------------------------===//
// Description of object. The only way to create an NValue is to call heap
// allocator and cast type.
//
// ***NEVER ADD VIRTUAL TO OBJECT MODEL***
//===----------------------------------------------------------------------===//
struct Bytecode;

template <typename AsType> inline AsType* as(void* addr) {
    return static_cast<AsType*>(addr);
}

template <typename IsType> inline bool is(void* ...addr) {
    return (as<IsType*>(addr)->getHeader()->template isType<IsType*>()&&...);
}

class NValue : public NonInstantiable {
  private:
    NHeader* header;

  public:
    inline NHeader* getHeader() { return header; }
};

class NInt : public NValue {
  private:
    int32 val;

  public:
    static int32 size() { return sizeof(NInt); }

    inline void setValue(int32 val) { this->val = val; }

    inline int32 getValue() const { return val; }
};

class NDouble : public NValue {
  private:
    int64 val;

  public:
    static int32 size() { return sizeof(NDouble); }
    inline void setValue(double val) { this->val = (int64)(val); }

    inline double getValue() const { return (double)val; }
};

class NChar : public NValue {
  private:
    int8 val;

  public:
    static int32 size() { return sizeof(NChar); }

    inline void setValue(int8 val) { this->val = val; }

    inline int8 getValue() const { return val; }
};

class NObject : public NValue {
  private:
    NType* type;

  public:
    inline void setType(NType* type) { this->type = type; }
};

class NArray: public NValue{
  private:
    uint32 length;
  public:
    static int32 size(uint32 len) { return sizeof(NArray)+sizeof(pointer)*len; }

    inline NValue* getElement(uint32 index){
        pointer addr = (pointer)this + sizeof(NArray)+ sizeof(pointer)*index;
        return as<NValue>(addr);
    }

    inline void setElement(uint32 index, NValue* value){
        pointer addr = (pointer)this + sizeof(NArray)+ sizeof(pointer)*index;
        as<NValue>(addr) = value;
    }

    inline void setLength(uint32 length){ this->length = length;}

    inline uint32 getLength() const{return this->length;}
};

#endif // NYX_NVALUE_H
