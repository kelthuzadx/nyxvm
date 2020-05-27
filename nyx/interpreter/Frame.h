#ifndef NYX_FRAME_H
#define NYX_FRAME_H

#include "../object/NValue.h"
#include <vector>

class Frame {
  private:
    std::vector<NValue*> slots;
    NValue** locals;

  public:
    explicit Frame(int localSize);

    ~Frame();

    NValue* pop();

    void push(NValue* obj);

    void load(int index);

    void store(int index, NValue* value);

    void dup();

    NValue** local() { return locals; }
};

#endif // NYX_FRAME_H
