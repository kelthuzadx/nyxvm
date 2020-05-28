#ifndef NYX_GENHEAP_H
#define NYX_GENHEAP_H

#include "../object/NValue.h"
#include "Space.h"
class GenHeap {
  private:
    static GenHeap heap;
    Space* youngSpace;
    Space* oldSpace;
    Space* fromSpace;
    Space* toSpace;
    std::vector<NType*> types;

  public:
    explicit GenHeap();
    ~GenHeap();

    static void initialize();

    static GenHeap instance() { return heap; }

  public:
    NInt* allocateNInt(int32 value);
    NDouble* allocateNDouble(double value);
    NChar* allocateNChar(int8 value);
    NObject* allocateNObject(NType* type);
    NArray* allocateNArray(uint32 length);
    NString* allocateNString(const std::string& str);
    NCallalbe* allocateNCallable(bool isNative, pointer ptr);
};

#endif // NYX_GENHEAP_H
