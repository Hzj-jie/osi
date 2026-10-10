
#ifndef B2STYLE_LIB_ASSERT_H
#define B2STYLE_LIB_ASSERT_H

#include <b2style/stdio.h>
#include <b2style/types.h>
#include <bstyle/str.h>

void assert(string statement, bool v, string msg) {
  if (v) return;
  if (str_empty(statement)) {
    b2style::std_out(msg);
  } else {
    b2style::std_out(statement + ": " + msg);
  }
  logic "stop";
}

void assert(bool v, string msg) {
  assert("", v, msg);
}

void assert(string statement, bool v) {
  assert(statement, v, "Assertion failure");
}

void assert(bool v) {
  assert("", v);
}

#endif  // B2STYLE_LIB_ASSERT_H