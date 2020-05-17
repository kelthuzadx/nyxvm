#include "MetaArea.h"

MetaArea::~MetaArea() {
    delete[] bytecodes;
}
