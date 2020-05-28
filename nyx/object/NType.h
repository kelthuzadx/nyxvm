#ifndef NYX_NTYPE_H
#define NYX_NTYPE_H

#include "../runtime/Global.h"
#include "NValue.h"
#include <cstdint>
#include <vector>

class NValue;

class NType {
  private:
    bool isArray;
    std::string name;
    std::vector<std::string> fields;

  public:
    explicit NType(std::string  name);
    void addField(const std::string& field);

    uint32 getInstanceSize() {
        return fields.size() * sizeof(pointer);
    }
};

#endif // NYX_NTYPE_H
