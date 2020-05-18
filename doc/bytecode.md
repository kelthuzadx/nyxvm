# nyx bytecode reference
This documentation describes nyx bytecode

## call
+ description: call the function either in this module or in one of imported modules
+ format: call,func_name_index,arg_num
+ stack: `(arg_n, arg_n-1,... arg2, arg1) -> ()`

## jmp
+ description: jump to target bytecode index
+ format: jmp,target_bci
+ stack: `()->()`

## jmp_eq
+ description: jump to target bytecode index if condition hols to true
+ format: jmp,target_bci
+ stack: `(cond)->()`

## jmp_ne
+ description: jump to target bytecode index if condition hols to false
+ format: jmp,target_bci
+ stack:  `(cond)->()`

## const_i
+ description: push an integer to stack
+ format: const_i, integer
+ stack: `()->(int)`

## const_null
+ description: push an null to stack
+ format: const_null
+ stack: `()->(null)`

## const_d
+ description: push a double to stack
+ format: const_d,double
+ stack: `()->(double)`

## const_str
+ description: push a string to stack
+ format: const_str,string_index
+ stack: `()->(string)`

## neg
+ description: negate value
+ format: neg
+ stack: `(val)->(val)`

## test_eq
+ description: test if left operand equals right operand
+ format: test_eq
+ stack: `(lhs,rhs)->(bool)`

## test_ne
+ description: test if left operand not equals right operand
+ format: test_ne
+ stack: `(lhs,rhs)->(bool)`

## test_ge
+ description: test if left operand greater equals right operand
+ format:test_ge
+ stack: `(lhs,rhs)->(bool)`

## test_gt
+ description: test if left operand greater than right operand
+ format:test_gt
+ stack: `(lhs,rhs)->(bool)`

## test_le
+ description: test if left operand less equals right operand
+ format:test_le
+ stack: `(lhs,rhs)->(bool)`

## test_lt
+ description:test if left operand less than right operand
+ format:test_lt
+ stack: `(lhs,rhs)->(bool)`

## and
+ description: bit and computation
+ format:and
+ stack:`(lhs,rhs)->(result)`

## or
+ description:bit or computation
+ format:or
+ stack:`(lhs,rhs)->(result)`

## not
+ description: bit not computation
+ format:not
+ stack:`(lhs)->(result)`

## add
+ description: addition operation
+ format: add
+ stack:`(lhs,rhs)->(result)`

## sub
+ description: subtraction operation
+ format:sub
+ stack:`(lhs,rhs)->(result)`

## mul
+ description: multiplication
+ format:mul
+ stack:`(lhs,rhs)->(result)`

## div
+ description: division
+ format: div
+ stack:`(lhs,rhs)->(result)`

## rem
+ description: remainder operation
+ format:rem
+ stack:`(lhs,rhs)->(result)`

## return
+ description: return to caller directly
+ format: return
+ stack: `()->()`

## return_val
+ description: return to caller with additional result
+ format: return_val
+ stack:`callstack(val)`

## load
+ description: load value from local table to stack
+ format: load,index
+ stack:`()->(val)`

## store
+ description: store stack top to local table
+ format: store,index
+ stack:`(val)->()`

## load_index
+ description: load element from array at given index
+ format: load_index
+ stack: `(array,index)->(elem)`

## store_index
+ description: store element to array at given index
+ format: store_index
+ stack:`(arry,elem,index)->()`

## new_arr
+ description: create new array and push to stack
+ format: new_arr,length
+ stack: `()->(arr)`

## dup
+ description: duplicate stack top
+ format:dup
+ stack:`(val)->(val,val)`

## arr_len
+ description: get length of array
+ format: arr_len
+ stack:`(arr)->(length)`

