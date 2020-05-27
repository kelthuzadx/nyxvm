#include "GenHeap.h"
#include <memory>

#define KB (1024)
#define MB (1024*KB)

GenHeap GenHeap::heap;

GenHeap::GenHeap() = default;

GenHeap::~GenHeap(){
    delete youngSpace;
    delete oldSpace;
    delete fromSpace;
    delete toSpace;
}

void GenHeap::initialize() {
    heap.youngSpace = new Space(50*MB);
    heap.oldSpace = new Space(50*MB);
    heap.fromSpace= new Space(10*MB);
    heap.toSpace = new Space(10*MB);
}
NInt* GenHeap::allocateNInt(int32 value) {
    int32 size = NInt::size();
    pointer addr = youngSpace->allocate(size);
    as<NInt>(addr)->getHeader()->setType(NHeader::TypeInt);
    as<NInt>(addr)->setValue(value);
    return as<NInt>(addr);
}
NInt* GenHeap::allocateNDouble(double value) { return nullptr; }
NInt* GenHeap::allocateNChar(int8 value) { return nullptr; }
NValue* GenHeap::allocateNObject(NType* type) { return nullptr; }
