#include "NValue.h"

NInt::NInt(int32 value) : value(value) {}

NDouble::NDouble(double value) : value(value) {}

NChar::NChar(int8 value) : value(value) {}

NString::NString(std::string value) : value(std::move(value)) {}

NArray::~NArray() { delete[] array; }

NArray::NArray(int length) {
    this->length = length;
    this->array = new NValue*[length];
}

NCallable::NCallable(Bytecode* code, bool isNative) : isNative(isNative) {
    this->code.bytecode = code;
}

NCallable::NCallable(const char* code, bool isNative) : isNative(isNative) {
    this->code.native = code;
}

NCallable::~NCallable() { this->code.native = nullptr; }

std::string NCallable::toString() { return "callable"; }

std::string NValue::toString() {
    int addr = *((int*)(this));
    std::string str("Object[");
    str += std::to_string(addr);
    str += "]";
    return str;
}

std::string NInt::toString() { return std::to_string(value); }

std::string NChar::toString() {
    return "'" + std::string(1, (char)value) + "'";
}

std::string NDouble::toString() { return std::to_string(value); }

std::string NString::toString() { return value; }

std::string NArray::toString() {
    std::string str("[");
    if (length >= 1) {
        for (int i = 0; i < length - 1; i++) {
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