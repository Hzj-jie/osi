
#ifndef B2STYLE_LIB_TESTING_ASSERT_H
#define B2STYLE_LIB_TESTING_ASSERT_H

#include <b2style/stdio.h>
#include <b2style/to_str.h>
#include <bstyle.h>
#include <bstyle/str.h>
#include <testing/types.h>

namespace b2style {
namespace testing {

int _assertion_count = 0;

// TODO: Find a better way to test the __FILE__ in included files.
string assert__FILE__() {
  return __FILE__;
}

void assert_true(bool v, string msg) {
  _assertion_count++;
  string prefix;
  if (v) {
    prefix = "Success: ";
  } else {
    prefix = "Failure: ";
  }
  ::b2style::std_out(prefix);
  ::b2style::std_out(msg);
  ::b2style::std_out("\n");
}

void assert_true(bool v) {
  assert_true(v, "no extra information.");
}

void assert_false(bool v, string msg) {
  assert_true(!v, msg);
}

void assert_false(bool v) {
  assert_true(!v);
}

/*
TODO: Make this work. Currently assert_equal__2 conflicts with the following one.
template <T, T2>
void assert_equal(T t, T2 t2, string msg) {
  assert_true(t == t2, msg);
}
*/

template <T, T2>
void assert_equal(T t, T2 t2) {
  assert_true(t == t2, ::str_concat(::b2style::to_str(t), " != ", ::b2style::to_str(t2)));
}

template <T>
void assert_equal(T t, T t2) {
  assert_equal<T, T>(t, t2);
}

}  // namespace testing
}  // namespace b2style

#endif  // B2STYLE_LIB_TESTING_ASSERT_H
