#ifndef NYX_FRAME_H
#define NYX_FRAME_H

#include <vector>
#include "../runtime/Object.h"

class Frame {
private:
    std::vector<Object *> slots;
    Object **locals;

public:
    explicit Frame(int localSize);

    ~Frame();

    Object *pop();

    void push(Object *obj);

    void load(int index);

    void store(int index, Object *value);

    void dup();

    Object** local(){return locals;}
};

#endif //NYX_FRAME_H
