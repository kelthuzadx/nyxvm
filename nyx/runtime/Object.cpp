#include "Object.h"

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
