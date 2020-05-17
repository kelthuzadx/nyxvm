#include "Frame.h"

Frame::Frame(int localSize) {
    local = new Object *[localSize];
}

Frame::~Frame() {
    delete[] local;
    for (auto &slot:slots) {
        delete slot;
    }
}

Object *Frame::pop() {
    Object *obj = slots.back();
    slots.pop_back();
    return obj;
}

void Frame::push(Object *obj) {
    slots.push_back(obj);
}

void Frame::load(int index) {
    slots.push_back(local[index]);
}

void Frame::store(int index, Object *value) {
    local[index] = value;
}

void Frame::dup() {
    Object *object = pop();
    push(object);
    push(object);
}
