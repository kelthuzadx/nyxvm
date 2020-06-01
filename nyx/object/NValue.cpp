#include "NValue.h"

void NString::initialize(uint32 length, int8* data) {
    this->length = length;
    int8* chars = reinterpret_cast<int8*>((pointer)this + sizeof(NString));
    for (int i = 0; i < length; i++) {
        chars[i] = data[i];
    }
}
std::string NString::getValue() {
    std::string str;
    for (int i = 0; i < length; i++) {
        str += (char)getData()[i];
    }
    return str;
}
void NArray::initialize(uint32 length) {
    this->length = length;
    for (int i = 0; i < length; i++) {
        setElement(i, (NValue*)0xdeadbeef);
    }
}
void NCallable::initialize(bool isNative, pointer code) {
    this->isNative = isNative;
    this->code = code;
}
