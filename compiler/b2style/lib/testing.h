
#ifndef TESTING_TESTING_H
#define TESTING_TESTING_H

#include <b2style/stdio.h>
#include <testing/types.h>
#include <testing/assert.h>

namespace b2style {
namespace testing {

void finished() {
  ::b2style::std_out("Total assertions: ");
  ::b2style::std_out(_assertion_count);
  ::b2style::std_out("\n");
}

}  // namespace testing
}  // namespace b2style

#endif  // TESTING_TESTING_H
