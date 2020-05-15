#ifndef NYX_OBJECT_H
#define NYX_OBJECT_H

#include "../runtime/Global.hpp"

struct Object {

    Object(){}
    virtual ~Object(){}
};

struct NInt:public Object{
    NInt(nyx::int32 value):value(value){}

    nyx::int32 value;
};

struct NDouble:public Object{
    NDouble(double value):value(value){}

    double value;
};


#endif //NYX_OBJECT_H
