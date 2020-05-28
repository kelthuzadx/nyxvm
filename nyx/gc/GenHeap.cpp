#include "GenHeap.h"
#include <memory>

#define KB (1024)
#define MB (1024 * KB)

GenHeap GenHeap::heap;

GenHeap::GenHeap() = default;

GenHeap::~GenHeap() {
    delete youngSpace;
    delete oldSpace;
    delete fromSpace;
    delete toSpace;
}

void GenHeap::initialize() {
    heap.youngSpace = new Space(50 * MB);
    heap.oldSpace = new Space(50 * MB);
    heap.fromSpace = new Space(10 * MB);
    heap.toSpace = new Space(10 * MB);

    NType* tString = new NType("string");
    tString->addField("length");
    tString->addField("data");

    heap.types.push_back(tString);
}
NInt* GenHeap::allocateNInt(int32 value) {
    int32 size = NInt::size();
    pointer addr = youngSpace->allocate(size);
    as<NInt>(addr)->getHeader()->setType<NInt>();
    as<NInt>(addr)->setValue(value);
    return as<NInt>(addr);
}
NDouble* GenHeap::allocateNDouble(double value) {
    int32 size = NDouble::size();
    pointer addr = youngSpace->allocate(size);
    as<NDouble>(addr)->getHeader()->setType<NDouble>();
    as<NDouble>(addr)->setValue(value);
    return as<NDouble>(addr);
}
NChar* GenHeap::allocateNChar(int8 value) {
    int32 size = NChar::size();
    pointer addr = youngSpace->allocate(size);
    as<NChar>(addr)->getHeader()->setType<NChar>();
    as<NChar>(addr)->setValue(value);
    return as<NChar>(addr);
}
NObject* GenHeap::allocateNObject(NType* type) {
    uint32 size = type->getInstanceSize();
    pointer addr = youngSpace->allocate(size);
    as<NObject>(addr)->getHeader()->setType<NObject>();
    as<NObject>(addr)->setType(type);
    return as<NObject>(addr);
}
