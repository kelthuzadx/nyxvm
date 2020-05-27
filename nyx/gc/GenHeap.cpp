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

}
