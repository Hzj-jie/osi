
#ifndef BSTYLE_LIB_LIMITS_H
#define BSTYLE_LIB_LIMITS_H

#include <bstyle/int.h>
#include <bstyle/types.h>

int INT_MIN = -2147483648;
int INT_MAX = 2147483647;
// TODO: Support negative long.
// long LONG_MIN = -9223372036854775808L;
long LONG_MAX = to_long(9223372036854775807L);

#endif  // BSTYLE_LIB_LIMITS_H