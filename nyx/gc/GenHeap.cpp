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
    for(auto&type:types){
        delete type;
    }
}

void GenHeap::initialize() {
    heap.youngSpace = new Space(50 * MB);
    heap.oldSpace = new Space(50 * MB);
    heap.fromSpace = new Space(10 * MB);
    heap.toSpace = new Space(10 * MB);
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
NArray* GenHeap::allocateNArray(uint32 length) {
    uint32 size = NArray::size(length);
    pointer addr = youngSpace->allocate(size);
    as<NArray>(addr)->initialize(length);
    return as<NArray>(addr);
}
NString* GenHeap::allocateNString(const std::string& str) {
    uint32 size = NString::size(str.size());
    pointer addr = youngSpace->allocate(size);
    as<NString>(addr)->initialize(size, (int8*)str.c_str());
    return as<NString>(addr);
}
NCallalbe* GenHeap::allocateNCallable(bool isNative, pointer ptr){
    uint32 size = NCallalbe::size();
    pointer addr = youngSpace->allocate(size);
    as<NCallalbe>(addr)->initialize(isNative,ptr);
    return as<NCallalbe>(addr);
}
