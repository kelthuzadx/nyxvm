#ifndef NYX_NTYPE_H
#define NYX_NTYPE_H

#include "../runtime/Global.h"
#include "NValue.h"
#include <cstdint>
#include <vector>

class NValue;

class NType {
  private:
    std::string name;
    std::map<int,std::string> fields;

  public:
    explicit NType(constd std::string& name);

    uint32 getInstanceSize(){
        return fields.size()* sizeof(pointer);
    }
};

#endif // NYX_NTYPE_H
