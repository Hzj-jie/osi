
#ifndef BSTYLE_LIB_BSTYLE_TYPES_H
#define BSTYLE_LIB_BSTYLE_TYPES_H

typedef Integer int;
logic "type long 8";
typedef Boolean bool;
logic "type byte 1";
typedef BigUnsignedInteger biguint;
typedef BigUnsignedFloat ufloat;
typedef String string;
// This constant needs to match the implementation in logic/scope.type_t.zero_type.
typedef type0 void;

#endif  // BSTYLE_LIB_BSTYLE_TYPES_H
