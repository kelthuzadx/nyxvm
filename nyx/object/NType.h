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
    std::map<int, std::string> fields;

  public:
    explicit NType(const std::string& name);

    void* operator new(size_t size) = delete;
    void operator delete(void* ptr) = delete;

    uint32 getInstanceSize() { return fields.size() * sizeof(pointer); }
};

#endif // NYX_NTYPE_H
