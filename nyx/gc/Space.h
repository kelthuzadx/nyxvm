#ifndef NYX_SPACE_H
#define NYX_SPACE_H

#include "../runtime/Global.h"
class Space {
  private:
    pointer base;
    int32 size;
    pointer top;

  public:
    Space(int32 size);
    ~Space();
};

#endif // NYX_SPACE_H
