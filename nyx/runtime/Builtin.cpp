#include <iostream>
#include "Object.h"

extern "C" Object *nyxffi_print(int argc, Object **argv) {
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i]->toString();
    }
    std::cout.flush();
    return nullptr;
}

extern "C" Object *nyxffi_println(int argc, Object **argv) {
    nyxffi_print(argc, argv);
    std::cout << "\n";
    std::cout.flush();
    return nullptr;
}

extern "C" Object *nyxffi_typeof(int argc, Object **argv) {
    assert(argc == 1);
    if (argv[0] == nullptr) {
        return new NString("null");
    } else if (typeid(*argv[0]) == typeid(NInt)) {
        return new NString("int");
    } else if (typeid(*argv[0]) == typeid(NDouble)) {
        return new NString("double");
    } else if (typeid(*argv[0]) == typeid(NString)) {
        return new NString("string");
    } else if (typeid(*argv[0]) == typeid(NArray)) {
        return new NString("array");
    }
    return new NString("object");
}

extern "C" Object *nyxffi_len(int argc, Object **argv) {
    assert(argc == 1 && (typeid(*argv[0]) == typeid(NArray) || typeid(*argv[0]) == typeid(NString)));
    if (typeid(*argv[0]) == typeid(NString)) {
        return new NInt(dynamic_cast<NString *>(argv[0])->value.length());
    } else if (typeid(*argv[0]) == typeid(NArray)) {
        return new NInt(dynamic_cast<NArray *>(argv[0])->length);
    }
    return nullptr;
}