#pragma once
#include <string>

namespace osi
{
    namespace compiler
    {
        namespace b3style_test_data
        {
            inline const std::string func_kw = R"B3STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

namespace N {
  ::void print() {
    ::b2style::std_out(__func__);
    ::b2style::std_out("\n");
  }

  ::void f2(::int x, ::string s) {
    ::b2style::std_out(__func__);
    ::b2style::std_out("\n");
  }
}  // namespace N

void main() {
  b2style::std_out(__func__);
  b2style::std_out("\n");
  N::print();
  N::f2(1, "");
}
)B3STYLE_EOF";

            inline const std::string destruction_in_declaration = R"B3STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class C {
  void construct() {
    b2style::std_out("construct");
  }

  void destruct() {
    b2style::std_out("destruct");
  }
};

void main() {
  C c;
  b2style::std_out("finish");
}
)B3STYLE_EOF";

            inline const std::string destruction_in_definition = R"B3STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class C {
  void construct() {
    b2style::std_out("construct");
  }

  void destruct() {
    b2style::std_out("destruct");
  }
};

C create() {
  b2style::std_out("create");
  C c;
  return c;
}

void main() {
  C c = create();
  b2style::std_out("finish");
}
)B3STYLE_EOF";

            inline const std::string errors_define_class_constructor_for_non_class = R"B3STYLE_EOF(
#include <b2style.h>

void main() {
  int x(100);
}
)B3STYLE_EOF";
        }
    }
}
