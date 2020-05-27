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
NDouble* GenHeap::allocateNDouble(double value) {
    int32 size = NDouble::size();
    pointer addr = youngSpace->allocate(size);
    as<NDouble>(addr)->getHeader()->setType(NHeader::TypeDouble);
    as<NDouble>(addr)->setValue(value);
    return as<NDouble>(addr);
}
NChar* GenHeap::allocateNChar(int8 value) {
    int32 size = NChar::size();
    pointer addr = youngSpace->allocate(size);
    as<NChar>(addr)->getHeader()->setType(NHeader::TypeChar);
    as<NChar>(addr)->setValue(value);
    return as<NChar>(addr);
}
NValue* GenHeap::allocateNObject(NType* type) { return nullptr; }
