#ifndef NYX_FRAME_H
#define NYX_FRAME_H

#include <vector>
#include "../runtime/Object.h"

struct Frame {
    explicit Frame(int localSize);

    ~Frame();

    std::vector<Object *> slots;
    Object **local;
};

#endif //NYX_FRAME_H
