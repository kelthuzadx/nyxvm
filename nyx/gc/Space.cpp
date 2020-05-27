#include "Space.h"
#include <sys/mman.h>

Space::Space(int32 size) {
    this->size = size;
    this->base = (pointer)::mmap(NULL, size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == -1) {
        panic("can not create space");
    }
    this->top = base;

}
Space::~Space() {
    ::munmap((void*)this->base,size);
}
