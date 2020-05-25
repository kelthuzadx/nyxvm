#ifndef NYX_NOBJECT_H
#define NYX_NOBJECT_H

#include <utility>

#include "Global.h"

struct Bytecode;

struct NObject {

    NObject() = default;

    virtual ~NObject() = default;

    virtual std::string toString();
};

struct NInt : public NObject {
    explicit NInt(nyx::int32 value);

    nyx::int32 value;

    std::string toString() override;
};

struct NDouble : public NObject {
    explicit NDouble(double value);

    double value;

    std::string toString() override;
};

struct NChar : public NObject {
    explicit NChar(nyx::int8 ch);

    nyx::int8 value;

    std::string toString() override;
};

struct NString : public NObject {
    explicit NString(std::string value);

    std::string value;

    std::string toString() override;
};

struct NArray : public NObject {
    explicit NArray(int length);

    ~NArray() override;

    std::string toString() override;

    int length;
    NObject** array;
};

struct NCallable : public NObject {
    explicit NCallable(Bytecode* code, bool isNative);

    explicit NCallable(const char* code, bool isNative);

    ~NCallable() override;

    std::string toString() override;

    bool isNative;

    union {
        Bytecode* bytecode;
        const char* native;
    } code{};
};

#endif // NYX_NOBJECT_H
