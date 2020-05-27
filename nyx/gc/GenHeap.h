#ifndef NYX_GENHEAP_H
#define NYX_GENHEAP_H

#include "../object/NObject.h"
#include "Space.h"
class GenHeap {
  private:
    static GenHeap heap;
    Space* youngSpace{};
    Space* oldSpace{};
    Space* fromSpace{};
    Space* toSpace{};

  public:
    explicit GenHeap();
    ~GenHeap();

    static void initialize();

    static GenHeap instance() { return heap; }

    NInt* allocateNInt(int32 value);
    NInt* allocateNDouble(double value);
    NInt* allocateNChar(int8 value);
    NObject* allocateNObject(NType* type);

};

#endif // NYX_GENHEAP_H
