#include "util/Utils.h"

#include "runtime/NyxVM.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        panic("Feed your *.nyx source file to interpreter!\n");
    }

    NyxVM vm;
    vm.setup();
    vm.ignition(argv[1]);
    return 0;
}
