#ifndef NYX_BYTECODEDUMP_H
#define NYX_BYTECODEDUMP_H

#include "Bytecode.h"
#include <fstream>

class BytecodeDump {
  private:
    std::ofstream ofs;
    PhaseTime timer;

  public:
    explicit BytecodeDump(const std::string& logPath);

    ~BytecodeDump();

    void dump(Bytecode* bytecode);
};

#endif // NYX_BYTECODEDUMP_H
