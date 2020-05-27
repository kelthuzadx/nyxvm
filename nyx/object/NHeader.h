#ifndef NYX_NHEADER_H
#define NYX_NHEADER_H

#include "../runtime/Global.h"
#include <cstdlib>
class NHeader {
  private:
    pointer * header;

  public:
    void* operator new(size_t size)= delete;
    void operator delete(void* ptr)= delete;
    explicit NHeader()= delete;
    ~NHeader()= delete;
};

#endif // NYX_NHEADER_H
