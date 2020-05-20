#include "Object.h"

NInt::NInt(nyx::int32 value) : value(value) {}

NDouble::NDouble(double value) : value(value) {}

NString::NString(std::string value) : value(std::move(value)) {}

NArray::~NArray() {
    delete[] array;
}


NArray::NArray(int length) {
    this->length = length;
    this->array = new Object *[length];
}


NClosure::NClosure(Bytecode *code):code(code) {

}


std::string Object::toString() {
    int addr = *((int *) (this));
    std::string str("Object[");
    str += std::to_string(addr);
    str += "]";
    return str;
}

std::string NInt::toString() {
    return std::to_string(value);
}


std::string NDouble::toString() {
    return std::to_string(value);
}

std::string NString::toString() {
    return value;
}

std::string NArray::toString() {
    std::string str("[");
    if (length >= 1) {
        for (int i = 0; i < length - 2; i++) {
            if (array[i] != nullptr) {
                str += array[i]->toString();
            } else {
                str += "null";
            }
            str += ",";
        }
        if (array[length - 1] != nullptr) {
            str += array[length - 1]->toString();
        } else {
            str += "null";
        }
    }
    str += "]";

    return str;
}
