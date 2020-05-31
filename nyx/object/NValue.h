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
// ***NEVER ADD VIRTUAL TO OBJECT MODEL, USE TEMPLATE WHEN POSSIBLE***
//===----------------------------------------------------------------------===//
struct Bytecode;
class NType;

template <typename AsType> inline AsType* as(void* addr) {
    return static_cast<AsType*>(addr);
}

template <typename IsType> inline bool is(void* addr) {
    return as<IsType*>(addr)->getHeader()->template isType<IsType*>();
}

//===----------------------------------------------------------------------===//
// NValue:
//      [header] object header
//===----------------------------------------------------------------------===//
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

//===----------------------------------------------------------------------===//
// NObject:
//      [header] object header
//      [ type ] type pointer,contains field information
//      [field1] field 1
//      [field2] field 2
//      [ ...  ] ...
//===----------------------------------------------------------------------===//
class NObject : public NValue {
  private:
    NType* type;

  public:
    inline void setType(NType* type) { this->type = type; }
};

//===----------------------------------------------------------------------===//
// NArray:
//      [header] object header
//      [length] length of array
//      [ elem1] element 1
//      [ elem2] element 2
//      [ ...  ] ...
//===----------------------------------------------------------------------===//
class NArray : public NValue {
  private:
    uint32 length;

  public:
    static int32 size(uint32 len) {
        return sizeof(NArray) + sizeof(pointer) * len;
    }

    inline NValue* getElement(uint32 index) {
        pointer addr = (pointer)this + sizeof(NArray) + sizeof(pointer) * index;
        return as<NValue>(addr);
    }

    inline void setElement(uint32 index, NValue* value) {
        pointer addr = (pointer)this + sizeof(NArray) + sizeof(pointer) * index;
        *addr = *(pointer)value;
    }

    void initialize(uint32 length);

    inline uint32 getLength() const { return this->length; }
};

//===----------------------------------------------------------------------===//
// NString:
//      [header] object header
//      [length] length of string
//      [ char1] char 1
//      [ char2] char 2
//      [ ...  ] ...
//===----------------------------------------------------------------------===//
class NString : public NValue {
  private:
    uint32 length;

  public:
    static int32 size(uint32 len) { return sizeof(NString) + len; }

    void initialize(uint32 length, int8* data);

    inline uint32 getLength() const { return this->length; }

    inline int8* getData() const {return sizeof(NString)+(pointer)this;}

    std::string getValue();
};

//===----------------------------------------------------------------------===//
// NCallable:
//      [header] object header
//      [is_ntv] is native function
//      [ ptr  ] points to function
//===----------------------------------------------------------------------===//
class NCallable : public NValue {
  private:
    bool isNative;
    pointer code;

  public:
    static int32 size() { return sizeof(NCallable); }

    void initialize(bool isNative, pointer code);

    inline Bytecode* getBytecodePtr() {
        return reinterpret_cast<Bytecode*>(code);
    }

    inline const char* getNativePtr() {
        return reinterpret_cast<const char*>(code);
    }

    inline bool isNativeCallable()const{return isNative;}
};
#endif // NYX_NVALUE_H
