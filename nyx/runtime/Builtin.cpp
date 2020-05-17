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
    if (typeid(*argv[0]) == typeid(NInt)) {
        return nullptr;
    }
    return nullptr;
}