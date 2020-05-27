#ifndef NYX_SPACE_H
#define NYX_SPACE_H

#include "../runtime/Global.h"
class Space {
  private:
    pointer base;
    uint32 size;
    pointer top;

  public:
    explicit Space(uint32 size);
    ~Space();

    pointer allocate(uint32 needed);
};

#endif // NYX_SPACE_H
