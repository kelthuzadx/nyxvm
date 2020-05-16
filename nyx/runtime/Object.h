#ifndef NYX_OBJECT_H
#define NYX_OBJECT_H

#include "Global.h"

struct Object {

    Object() = default;

    virtual ~Object() = default;

    virtual std::string toString();
};

struct NInt : public Object {
    explicit NInt(nyx::int32 value) : value(value) {}

    nyx::int32 value;

    std::string toString() override;
};

struct NDouble : public Object {
    explicit NDouble(double value) : value(value) {}

    double value;

    std::string toString() override;
};


#endif //NYX_OBJECT_H
