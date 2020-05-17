#ifndef NYX_BYTECODEDUMP_H
#define NYX_BYTECODEDUMP_H

#include <fstream>
#include "Bytecode.h"

class BytecodeDump {
private:
    std::ofstream ofs;
public:
    BytecodeDump(const std::string &logPath);

    ~BytecodeDump();

    void dump(Bytecode *bytecode);
};


#endif //NYX_BYTECODEDUMP_H
