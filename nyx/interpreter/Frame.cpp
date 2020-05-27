#include "Frame.h"

Frame::Frame(int localSize) { locals = new NValue*[localSize]; }

Frame::~Frame() {
    delete[] locals;
}

NValue* Frame::pop() {
    NValue* obj = slots.back();
    slots.pop_back();
    return obj;
}

void Frame::push(NValue* obj) { slots.push_back(obj); }

void Frame::load(int index) { slots.push_back(locals[index]); }

void Frame::store(int index, NValue* value) { locals[index] = value; }

void Frame::dup() {
    NValue* object = pop();
    push(object);
    push(object);
}