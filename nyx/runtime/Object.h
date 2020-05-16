#ifndef NYX_OBJECT_H
#define NYX_OBJECT_H

#include "Global.hpp"

struct Object {

    Object() {}

    virtual ~Object() {}

    virtual std::string toString();
};

struct NInt : public Object {
    NInt(nyx::int32 value) : value(value) {}

    nyx::int32 value;

    std::string toString() override;
};

struct NDouble : public Object {
    NDouble(double value) : value(value) {}

    double value;

    std::string toString() override;
};


#endif //NYX_OBJECT_H
