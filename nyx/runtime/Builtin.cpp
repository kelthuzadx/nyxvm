#include "NObject.h"
#include <iostream>

extern "C" NObject* nyxffi_print(int argc, NObject** argv) {
    for (int i = 0; i < argc; i++) {
        if (argv[i] != nullptr) {
            std::cout << argv[i]->toString();
        } else {
            std::cout << "null";
        }
    }
    std::cout.flush();
    return nullptr;
}

extern "C" NObject* nyxffi_println(int argc, NObject** argv) {
    nyxffi_print(argc, argv);
    std::cout << "\n";
    std::cout.flush();
    return nullptr;
}

extern "C" NObject* nyxffi_typeof(int argc, NObject** argv) {
    assert(argc == 1);
    if (argv[0] == nullptr) {
        return new NString("null");
    } else if (typeid(*argv[0]) == typeid(NInt)) {
        return new NString("int");
    } else if (typeid(*argv[0]) == typeid(NChar)) {
        return new NString("char");
    } else if (typeid(*argv[0]) == typeid(NDouble)) {
        return new NString("double");
    } else if (typeid(*argv[0]) == typeid(NString)) {
        return new NString("string");
    } else if (typeid(*argv[0]) == typeid(NArray)) {
        return new NString("array");
    } else if (typeid(*argv[0]) == typeid(NCallable)) {
        return new NString("callable");
    }
    return new NString("object");
}

extern "C" NObject* nyxffi_len(int argc, NObject** argv) {
    assert(argc == 1 && (typeid(*argv[0]) == typeid(NArray) ||
                         typeid(*argv[0]) == typeid(NString)));
    if (typeid(*argv[0]) == typeid(NString)) {
        return new NInt(dynamic_cast<NString*>(argv[0])->value.length());
    } else if (typeid(*argv[0]) == typeid(NArray)) {
        return new NInt(dynamic_cast<NArray*>(argv[0])->length);
    }
    return nullptr;
}

extern "C" NObject* nyxffi_exit(int argc, NObject** argv) {
    assert(argc == 1 && typeid(*argv[0]) == typeid(NInt));
    exit(dynamic_cast<NInt*>(argv[0])->value);
    return nullptr;
}

extern "C" NObject* nyxffi_assert(int argc, NObject** argv) {
    assert((argc == 1 || argc == 2) && typeid(*argv[0]) == typeid(NInt));
    if (dynamic_cast<NInt*>(argv[0])->value == 0) {
        if (argc == 2 && typeid(*argv[1]) == typeid(NString)) {
            std::cerr << dynamic_cast<NString*>(argv[1])->value << "\n";
        } else {
            std::cerr << "Assertion failure!\n";
        }
        exit(1);
    }
    return nullptr;
}

extern "C" NObject* nyxffi_range(int argc, NObject** argv) {
    assert(argc == 2 || argc == 1);
    NInt* start = nullptr;
    NInt* end = nullptr;
    if (argc == 2) {
        start = dynamic_cast<NInt*>(argv[0]);
        end = dynamic_cast<NInt*>(argv[1]);
    } else {
        auto* t = dynamic_cast<NInt*>(argv[0]);
        if (t->value >= 0) {
            start = new NInt(0);
            end = t;
        } else {
            start = t;
            end = new NInt(0);
        }
    }
    auto* arr = new NArray(std::abs(start->value - end->value));
    for (int i = 0; std::abs(start->value - end->value) > i; i++) {
        arr->array[i] = new NInt(i);
    }
    return arr;
}