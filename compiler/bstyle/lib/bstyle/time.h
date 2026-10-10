
#ifndef BSTYLE_LIB_BSTYLE_TIME_H
#define BSTYLE_LIB_BSTYLE_TIME_H

#include <bstyle/const.h>
#include <bstyle/types.h>

int current_ms() {
  int result;
  logic "interrupt current_ms @@prefixes@temps@string result";
  return result;
}

#endif  // BSTYLE_LIB_BSTYLE_TIME_H