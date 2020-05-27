#ifndef NYX_NHEADER_H
#define NYX_NHEADER_H

#include "../runtime/Global.h"
#include "NObject.h"
#include <cstdlib>

//===----------------------------------------------------------------------===//
// [_][_][_][_][_][_][_][_][_][_]
//===----------------------------------------------------------------------===//
class NHeader :public NonInstantiable{
  private:
    pointer * word;

  public:
    enum HeaderType{
        TypeInt,
        TypeDouble,
        TypeChar,
        TypeObject,
    };


    void setType(HeaderType type){
        (*word) = ((word)&type);
    }

    bool isType(HeaderType type){
        return (*word)
    }
};

#endif // NYX_NHEADER_H
