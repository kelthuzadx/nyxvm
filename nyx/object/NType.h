#ifndef NYX_NTYPE_H
#define NYX_NTYPE_H

#include "../runtime/Global.h"
#include "NObject.h"
#include <cstdint>
#include <vector>

class NType {
  private:
    bool isArray;
    bool isCallable;
    bool isString;
    int32 size;
    std::vector<NObject*> fields;

  public:


};

#endif // NYX_NTYPE_H
