#include "Builtin.h"
#include <iostream>

void nyxffi_println(int argc, Object **argv) {
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i]->toString();
    }
    std::cout << "\n";
}