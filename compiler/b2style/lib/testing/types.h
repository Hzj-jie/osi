
#ifndef B2STYLE_LIB_TESTING_TYPES_H
#define B2STYLE_LIB_TESTING_TYPES_H

#include <bstyle/types.h>

namespace b2style {
namespace testing {

typedef ::string string;
typedef ::void void;
typedef ::bool bool;
typedef ::int int;

// TODO: Find a better way to test the __FILE__ in included files.
string types__FILE__() {
  return __FILE__;
}

}  // namespace testing
}  // naemspace b2style

#endif  // B2STYLE_LIB_TESTING_TYPES_H