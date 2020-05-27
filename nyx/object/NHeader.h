#ifndef NYX_NHEADER_H
#define NYX_NHEADER_H

#include "../runtime/Global.h"
#include "NValue.h"
#include <cstdlib>

//===----------------------------------------------------------------------===//
// [_][_][_][_][_][_][_][_][_][_]
//===----------------------------------------------------------------------===//
class NHeader :public NonInstantiable{
  private:
    pointer * header;

  public:
    enum HeaderType{
        TypeInt,
        TypeDouble,
        TypeChar,
        TypeObject,
    };


    void setType(HeaderType type){
        (*header) = ((*header)&(type));
    }

    bool isType(HeaderType type){
        return ((*header)&(type));
    }
};

#endif // NYX_NHEADER_H
