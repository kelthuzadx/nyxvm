#include "Frame.h"

Frame::Frame(int localSize) { locals = new NObject*[localSize]; }

Frame::~Frame() {
    delete[] locals;
    for (auto& slot : slots) {
        delete slot;
    }
}

NObject* Frame::pop() {
    NObject* obj = slots.back();
    slots.pop_back();
    return obj;
}

void Frame::push(NObject* obj) { slots.push_back(obj); }

void Frame::load(int index) { slots.push_back(locals[index]); }

void Frame::store(int index, NObject* value) { locals[index] = value; }

void Frame::dup() {
    NObject* object = pop();
    push(object);
    push(object);
}