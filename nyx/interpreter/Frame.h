#ifndef NYX_FRAME_H
#define NYX_FRAME_H

#include "../object/NObject.h"
#include <vector>

class Frame {
  private:
    std::vector<NObject*> slots;
    NObject** locals;

  public:
    explicit Frame(int localSize);

    ~Frame();

    NObject* pop();

    void push(NObject* obj);

    void load(int index);

    void store(int index, NObject* value);

    void dup();

    NObject** local() { return locals; }
};

#endif // NYX_FRAME_H
