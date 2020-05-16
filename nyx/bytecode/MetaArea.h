#ifndef NYX_METAAREA_H
#define NYX_METAAREA_H

#include <vector>
#include <string>
#include "../runtime/Global.h"

struct MetaArea {

    int bytecodeSize;
    nyx::int32 *bytecodes;
    std::vector<std::string> strings;
};


#endif //NYX_METAAREA_H
