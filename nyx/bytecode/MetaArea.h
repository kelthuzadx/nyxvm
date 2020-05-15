#ifndef NYX_METAAREA_H
#define NYX_METAAREA_H

#include <vector>
#include <string>
#include "../runtime/Global.hpp"

struct MetaArea {

    int bytecodeSize;
    nyx::int8 *bytecodes;
    std::vector<std::string> strings;
};


#endif //NYX_METAAREA_H
