#include "Frame.h"

Frame::Frame(int localSize) {
    local = new Object *[localSize];
}

Frame::~Frame() {
    delete[] local;
}
