#include "../gc/GenHeap.h"
#include "../object/NValue.h"
#include <iostream>

extern "C" NValue* nyxffi_print(int argc, NValue** argv) {
    for (int i = 0; i < argc; i++) {
        if (argv[0] == nullptr) {
            std::cout << "null";
        } else if (is<NInt>(argv[0])) {
            std::cout << as<NInt>(argv[0])->getValue();
        } else if (is<NChar>(argv[0])) {
            std::cout << as<NChar>(argv[0])->getValue();
        } else if (is<NDouble>(argv[0])) {
            std::cout << as<NDouble>(argv[0])->getValue();
        } else if (is<NString>(argv[0])) {
            std::cout << as<NString>(argv[0])->getValue();
        } else if (is<NArray>(argv[0])) {
            std::cout << "array"; // todo:pretty
        } else if (is<NCallable>(argv[0])) {
            std::cout << "callable"; // todo:pretty
        } else {
            panic("should not reach here");
        }
    }
    std::cout.flush();
    return nullptr;
}

extern "C" NValue* nyxffi_println(int argc, NValue** argv) {
    nyxffi_print(argc, argv);
    std::cout << "\n";
    std::cout.flush();
    return nullptr;
}

extern "C" NValue* nyxffi_typeof(int argc, NValue** argv) {
    assert(argc == 1);
    if (argv[0] == nullptr) {
        return GenHeap::instance().allocateNString("null");
    } else if (is<NInt>(argv[0])) {
        return GenHeap::instance().allocateNString("int");
    } else if (is<NChar>(argv[0])) {
        return GenHeap::instance().allocateNString("char");
    } else if (is<NDouble>(argv[0])) {
        return GenHeap::instance().allocateNString("double");
    } else if (is<NString>(argv[0])) {
        return GenHeap::instance().allocateNString("string");
    } else if (is<NArray>(argv[0])) {
        return GenHeap::instance().allocateNString("array");
    } else if (is<NCallable>(argv[0])) {
        return GenHeap::instance().allocateNString("callable");
    }
    return GenHeap::instance().allocateNString("object");
}

extern "C" NValue* nyxffi_len(int argc, NValue** argv) {
    assert(argc == 1 && (is<NArray>(argv[0]) || is<NString>(argv[0])));
    if (is<NString>(argv[0])) {
        return GenHeap::instance().allocateNInt(
            as<NString>(argv[0])->getLength());
    } else if (is<NArray>(argv[0])) {
        return GenHeap::instance().allocateNInt(
            as<NArray>(argv[0])->getLength());
    }
    return nullptr;
}

extern "C" NValue* nyxffi_exit(int argc, NValue** argv) {
    assert(argc == 1 && is<NInt>(argv[0]));
    exit(as<NInt>(argv[0])->getValue());
    return nullptr;
}

extern "C" NValue* nyxffi_assert(int argc, NValue** argv) {
    assert((argc == 1 || argc == 2) && typeid(*argv[0]) == typeid(NInt));
    if (is<NInt>(argv[0]) && as<NInt>(argv[0])->getValue() == 0) {
        if (argc == 2 && typeid(*argv[1]) == typeid(NString)) {
            std::cerr << as<NString>(argv[1])->getValue() << "\n";
        } else {
            std::cerr << "Assertion failure!\n";
        }
        exit(1);
    }
    return nullptr;
}

extern "C" NValue* nyxffi_range(int argc, NValue** argv) {
    assert(argc == 2 || argc == 1);
    NInt* start = nullptr;
    NInt* end = nullptr;
    if (argc == 2) {
        start = as<NInt>(argv[0]);
        end = as<NInt>(argv[1]);
    } else {
        auto* t = as<NInt>(argv[0]);
        if (t->getValue() >= 0) {
            start = GenHeap::instance().allocateNInt(0);
            end = t;
        } else {
            start = t;
            end = GenHeap::instance().allocateNInt(0);
        }
    }
    auto* arr = GenHeap::instance().allocateNArray(
        std::abs(start->getValue() - end->getValue()));
    for (int i = 0; std::abs(start->getValue() - end->getValue()) > i; i++) {
        arr->setElement(i, GenHeap::instance().allocateNInt(i));
    }
    return arr;
}