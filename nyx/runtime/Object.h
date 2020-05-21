#ifndef NYX_OBJECT_H
#define NYX_OBJECT_H

#include <utility>

#include "Global.h"

struct Bytecode;

struct Object {

    Object() = default;

    virtual ~Object() = default;

    virtual std::string toString();
};

struct NInt : public Object {
    explicit NInt(nyx::int32 value);

    nyx::int32 value;

    std::string toString() override;
};

struct NDouble : public Object {
    explicit NDouble(double value);

    double value;

    std::string toString() override;
};

struct NString : public Object {
    explicit NString(std::string value);

    std::string value;

    std::string toString() override;
};

struct NArray : public Object {
    explicit NArray(int length);

    ~NArray() override;

    std::string toString() override;

    int length;
    Object **array;
};

struct NClosure : public Object {
    explicit NClosure(Bytecode *code);

    ~NClosure() override;

    std::string toString() override;

    Bytecode *code;
};

#endif //NYX_OBJECT_H
