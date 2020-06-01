#ifndef NYX_NONINSTANTIABLE_H
#define NYX_NONINSTANTIABLE_H

#include <cstdlib>

class NonInstantiable {
  public:
    void* operator new(size_t size) = delete;
    void operator delete(void* ptr) = delete;
    explicit NonInstantiable() = delete;
    ~NonInstantiable() { /*do nothing*/
    }
};

#endif // NYX_NONINSTANTIABLE_H
