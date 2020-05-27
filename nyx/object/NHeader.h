#ifndef NYX_NHEADER_H
#define NYX_NHEADER_H

#include "../runtime/Global.h"
#include "NValue.h"
#include "NonInstantiable.h"
#include <cstdlib>
#include <type_traits>

//===----------------------------------------------------------------------===//
// [_][_][_][_][_][_][_][_][_][_]
//===----------------------------------------------------------------------===//
class NInt;
class NDouble;
class NChar;
class NObject;

class NHeader :public NonInstantiable{
  private:
    pointer header;

  private:
    decltype(auto) value(){return *header;}

  public:
    enum HeaderType{
        TypeInt,
        TypeDouble,
        TypeChar,
        TypeObject,
    };


    template <typename Type>
    void setType(HeaderType type){
        if constexpr (std::is_same_v<Type,NInt*>){
            value() = value()& 0;
        }else if constexpr (std::is_same_v<Type,NDouble*>){
            value() = value()&1;
        }else if constexpr (std::is_same_v<Type,NChar*>){
            value() = value()&2;
        }else if constexpr (std::is_same_v<Type,NObject*>){
            value() = value()&3;
        }else{
            static_assert(false);
        }1001
    }

    template <typename Type>
    bool isType(){
        if constexpr (std::is_same_v<Type,NInt*>){
           return (value()&0xF) & 0;
        }else if constexpr (std::is_same_v<Type,NDouble*>){
            return (value()&0xF) & 1;
        }else if constexpr (std::is_same_v<Type,NChar*>){
            return (value()&0xF) & 2;
        }else if constexpr (std::is_same_v<Type,NObject*>){
            return (value()&0xF) & 3;
        }else{
            static_assert(false);
        }
    }
};

#endif // NYX_NHEADER_H
