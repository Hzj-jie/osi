#pragma once
#include <string>

namespace osi
{
    namespace compiler
    {
        namespace b2style_test_data
        {
            inline const std::string func_kw = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class C {
  void std_out() {
    b2style::std_out(__func__);
    b2style::std_out("\n");
  }
};

namespace N {
  ::void print() {
    ::b2style::std_out(__func__);
    ::b2style::std_out("\n");
  }

  ::void f2(::int x, ::string s) {
    ::b2style::std_out(__func__);
    ::b2style::std_out("\n");
  }
};

void main() {
  b2style::std_out(__func__);
  b2style::std_out("\n");
  C c;
  c.std_out();
  N::print();
  N::f2(1, "");
})B2STYLE_EOF";

            inline const std::string _1_to_100 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  int x = 0;
  for (int i = 0; i < 100; i += 1) {
    x = x + i + 1;
  }
  b2style::std_out(x);
  b2style::std_out("\n");
}
)B2STYLE_EOF";

            inline const std::string __i = R"B2STYLE_EOF(
#include <b2style.h>
#include <stdio.h>

void main() {
  int i = 0;
  ++i;
  putchar(i);
  ++i;
  putchar(i);
  ++i;
  putchar(i);
  ++i;
  putchar(i);
}
)B2STYLE_EOF";

            inline const std::string __i__ = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  int i = 1;
  i += i++;
  b2style::std_out(i);
  b2style::std_out("_");
  i += ++i;
  b2style::std_out(i);
  b2style::std_out("_");
  ++i;
  i += i++;
  b2style::std_out(i);
  b2style::std_out("_");
  ++i;
  b2style::std_out(i);
  b2style::std_out("_");
  i++;
  b2style::std_out(i);
  b2style::std_out("_");

  // This is not supported by c++ since the (++i) is a rvalue and (++i)++ does not take effect to the original i.
  // But b2style does not detect rvalue reference now, so the expected result is to increase i by 1 rather than 2.
  ++i++;
  b2style::std_out(i);
}
)B2STYLE_EOF";

            inline const std::string another_1_to_100 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  int x = 0;
  for (int i = 1; i <= 100; i += 1) {
    x += i;
  }
  b2style::std_out(x);
  b2style::std_out("\n");
}
)B2STYLE_EOF";

            inline const std::string assert_ = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>
#include <assert.h>

void main() {
  assert(true, "This assertion should pass.");
  assert(false, "This assertion should not pass.");
  b2style::std_out("This line should not be executed.");
}
)B2STYLE_EOF";

            inline const std::string assert_with_statement = R"B2STYLE_EOF(
#include <b2style.h>
#include <assert.h>

void main() {
  int i = 100;
  assert(__STATEMENT__, i >= 100, "line 7");
  assert(__STATEMENT__, i < 100, "line 8");
}
)B2STYLE_EOF";

            inline const std::string biguint = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  biguint i = 4294967296L;
  biguint j = 4294967296l;
  biguint k = 1L;
  b2style::std_out(i);
  b2style::std_out(j);
  b2style::std_out(k);
}
)B2STYLE_EOF";

            inline const std::string bool_and_bool = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  b2style::std_out(true && true);
  b2style::std_out(true && false);
  b2style::std_out(false && true);
  b2style::std_out(false && false);
  b2style::std_out(true || true);
  b2style::std_out(true || false);
  b2style::std_out(false || true);
  b2style::std_out(false || false);
}
)B2STYLE_EOF";

            inline const std::string calculate_pi_bbp = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  ufloat s = 0.0;
  for (int i = 0; i < 55; i += 1) {
    ufloat c = b2style::ufloat::fraction(4, 1 + (8 * i));
	c -= b2style::ufloat::fraction(2, 4 + (8 * i));
	c -= b2style::ufloat::fraction(1, 5 + (8 * i));
	c -= b2style::ufloat::fraction(1, 6 + (8 * i));
	biguint b = 1;
	c *= b2style::ufloat::fraction(1, b << (4 * i));
	s += c;
  }
  b2style::std_out(s);
}
)B2STYLE_EOF";

            inline const std::string case1 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  b2style::std_out("Hello World");
}
)B2STYLE_EOF";

            inline const std::string case2 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>
#include <bstyle/time.h>

bool is_even(long i) {
  long _2 = 2;
  long _0 = 0;
  return b2style::equal(b2style::mod(i, _2), _0);
}

void main() {
  for (int i = 0; i < 100; i += 1) {
    long x = current_ms();
    if (is_even(x)) {
      x = --x;
    }
    b2style::std_out(is_even(x));
  }
}
)B2STYLE_EOF";

            inline const std::string class_ = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class C {
  int x;

  void add_x(int y) {
    this.x += y;
  }

  int with_x(int y) {
    return this.x + y;
  }

  int return_x() {
    return this.x;
  }
};

void main() {
  C c;
  c.add_x(100);
  b2style::std_out(c.with_x(100));
  b2style::std_out(c.return_x());
}
)B2STYLE_EOF";

            inline const std::string class_function_with_namespace = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class C {
  int N::x;

  void N::print() {
    b2style::std_out(this.N::x);
  }
};

void main() {
  C c;
  c.N::x = 100;
  c.N::print();
  c.::N::x = 200;
  c.::N::print();
}
)B2STYLE_EOF";

            inline const std::string class_in_namespace = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

namespace N {

class C {
  ::int x;

  ::void print() {
    ::b2style::std_out(this.x);
  }
};

C c;

::void print() {
  ::N::c.x = 2;
  ::N::c.print();
}

}  // namespace N

namespace M {

::N::C c;

::void print() {
  ::M::c.x = 3;
  ::M::c.print();
}

}  // namespace M

void main() {
  N::C c;
  c.x = 1;
  c.print();
  N::print();
  M::print();
}
)B2STYLE_EOF";

            inline const std::string class_inheritance = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class B {
  int x;

  overridable int f() {
    return this.x;
  }

  void f2() {
    b2style::std_out("f2");
  }
};

class C : B {
  int y;

  override int f() {
    return this.x + this.y;
  }
};

void main() {
  C c;
  c.x=1;
  c.y=2;
  b2style::std_out(c.f());
  c.f2();
  reinterpret_cast(c, B);
  b2style::std_out(c.f());
  c.f2();
}
)B2STYLE_EOF";

            inline const std::string class_on_heap = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class C {
  int x;
  string y;

  void print() {
    b2style::std_out(this.x);
	b2style::std_out(" ");
	b2style::std_out(this.y);
  }
};

class C2 {
  C x;
  int y;

  void print() {
    this.x.print();
	b2style::std_out(" ");
	b2style::std_out(this.y);
	b2style::std_out("\n");
  }
};

void main() {
  C2 cs[100];
  for (int i = 0; i < 100; i++) {
    cs[i].x.x = i;
    cs[i].x.y = b2style::int_to_str(i);
	cs[i].y = i;
  }

  for (int i = 0; i < 100; i++) {
    cs[i].print();
  }
}
)B2STYLE_EOF";

            inline const std::string class_constructor = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

int destructs = 0;

class C {
  int x;

  void construct(int x) {
    this.x = x;
  }

  void construct() {
    this.construct(100);
  }

  void destruct() {
    ::destructs++;
  }
};

void main() {
  {
    C c();
    b2style::std_out(c.x);
  }
  b2style::std_out(::destructs);
  {
    C c(200);
	b2style::std_out(c.x);
  }
  b2style::std_out(::destructs);
}
)B2STYLE_EOF";

            inline const std::string comments = R"B2STYLE_EOF(
#include <b2style/operators.h>
#include <stdio.h> // comment can be placed here.

void main( /* LOL */ ) { /* Comments can be added here as well.
* This is a multi line comment.
*/
  int i = getchar();
  while (i != eof()) {
	putchar(i);  // comment can be placed here as well.
	i = getchar();
  }
}  // also here.
)B2STYLE_EOF";

            inline const std::string delegate_ = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

delegate int func(int, int&);

void main() {
  b2style::std_out("abc");
}
)B2STYLE_EOF";

            inline const std::string delegate_ref = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

template <RT, T>
delegate RT ref_function(T&);

int func(int& x) {
  return x++;
}

void main() {
  int i = 100;
  ref_function<int, int> f = func;
  b2style::std_out(f(i));
  b2style::std_out(i);
}
)B2STYLE_EOF";

            inline const std::string delegate_template = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

template<RT, T>
delegate RT function(T);

int inc(int i) {
  return ++i;
}

void write(string s) {
  b2style::std_out(s);
}

void self_inc(int& i) {
  i++;
}

void main() {
  function<int, int> f = inc;
  b2style::std_out(f(1));

  function<void, string> f2 = write;
  f2("abc");

  function<void, int&> f3 = self_inc;
  int i = 0;
  f3(i);
  b2style::std_out(i);
}
)B2STYLE_EOF";

            inline const std::string delegate2 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

delegate int func(int);

int f(int x) {
  return x + 1;
}

int g(int x) {
  return x - 1;
}

void main() {
  func a = f;
  b2style::std_out(a(2));
  a = g;
  b2style::std_out(a(2));
}
)B2STYLE_EOF";

            inline const std::string empty_struct = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

struct S {};

void f(S s) {
  b2style::std_out("f");
}

void f2(S& s) {
  b2style::std_out("f2");
}

S f3() {
  b2style::std_out("f3");
  S s;
  return s;
}

void main() {
  S s = f3();
  f(s);
  f2(s);
}
)B2STYLE_EOF";

            inline const std::string for_loop = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void self_increment(int& x) {
  x++;
}

void self_increment2(int& x) {
  ++x;
}

void main() {
  int r = 0;
  for (int i = 0; i < 100; i++) {
    r += i;
  }
  b2style::std_out(r);
  for (int i = 0; i < 100; ++i) {
    r += i;
  }
  b2style::std_out(r);
  for (int i = 0; i < 100; self_increment(i)) {
    r += i;
  }
  b2style::std_out(r);
  for (int i = 0; i < 100; self_increment2(i)) {
    r += i;
  }
  b2style::std_out(r);
}
)B2STYLE_EOF";

            inline const std::string function_ptr = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

/*
logic:
  callee_ref func__2__int__int& int ( x int& )
*/
template <T, RT>
delegate RT func(T);

int f1(int& x) {
  x++;
  return x;
}

int f2(int& x) {
  x--;
  return x;
}

void main() {
  func<int&, int> a = f1;
  b2style::std_out(a(100));
  a = f2;
  b2style::std_out(a(100));
}

)B2STYLE_EOF";

            inline const std::string function_ref = R"B2STYLE_EOF(
#include <b2style.h>

void self_increment(int& x) {
  x = x + 1;
}

int self_increment2(int& x) {
  x = x + 1;
  return x;
}

int self_increment3(int& x) {
  return self_increment2(x);
}

void self_increment4(int& x) {
  self_increment3(x);
}

int self_increment5(int& x) {
  int r = x;
  x = x + 1;
  return r;
}

void main() {
  int x = 0;
  self_increment(x);
  b2style::std_out(x);
  self_increment(x);
  b2style::std_out(x);
  b2style::std_out(self_increment2(x));
  b2style::std_out(self_increment2(x));
  b2style::std_out(self_increment3(x));
  self_increment4(x);
  b2style::std_out(x);
  self_increment5(x);
  b2style::std_out(x);
  b2style::std_out(self_increment5(x));
})B2STYLE_EOF";

            inline const std::string function_with_global_namespace = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

namespace N {

::void f() {
  ::b2style::std_out("f");
}

::void ::f() {
  ::b2style::std_out("::f");
}

}  // namespace N

void main() {
  N::f();
  f();
}
)B2STYLE_EOF";

            inline const std::string heap = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

struct S {
  string s;
};

void f(S s) {
  b2style::std_out(s.s);
}

S new(string s) {
  S r;
  r.s = s;
  return r;
}

void main() {
  int x = 1;
  int v[x++];
  S v2[1 + ++x++];
  v[0] = 100;
  v2[0] = new("abc");
  v2[1] = new("def");
  f(v2[0]);
  f(v2[1]);
  v2[2].s = "ghi";
  f(v2[2]);
  // TODO: Make v2[2].  s = "ghi" work.
  b2style::std_out(v[0]);
}
)B2STYLE_EOF";

            inline const std::string heap_declaration = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

struct S {
  string s;
};

void f(S s) {
  b2style::std_out(s.s);
}

S new(string s) {
  S r;
  r.s = s;
  return r;
}

void main() {
  int x = 0;
  int v[++x];
  S v2[++x];
}
)B2STYLE_EOF";

            inline const std::string heap_function_ref = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void f(int& x) {
  x++;
}

void main() {
  int s[100];
  for (int i = 0; i < 100; i++) {
    s[i] = i;
	f(s[i]);
  }

  for (int i = 0; i < 100; i++) {
    b2style::std_out(s[i]);
	b2style::std_out("\n");
  }
}
)B2STYLE_EOF";

            inline const std::string heap_ptr_to_int64 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

biguint from_ptr(type_ptr x) {
  return x;
}

void b2style::std_out(type_ptr x) {
  b2style::std_out(from_ptr(x));
}

void main() {
  int x[100];
  x[0] = 100;
  biguint y = x;
  int l[100];
  l[0] = 101;
  int o[100];
  o[0] = 102;
  b2style::std_out(y);
  b2style::std_out(" ");
  reinterpret_cast(l, type_ptr);
  b2style::std_out(from_ptr(l));
  b2style::std_out(" ");
  reinterpret_cast(o, type_ptr);
  b2style::std_out(o);
}
)B2STYLE_EOF";

            inline const std::string i__ = R"B2STYLE_EOF(
typedef Integer int;
typedef type0 void;
typedef String string;

namespace b2style {

typedef ::int int;

#ifdef B2STYLE
int self_inc_post(int& x) {
  int r = x;
  int y = 1;
  logic "add b2style__x b2style__x b2style__y";
  return r;
}
#else
int self_inc_post(int& x) {
  int r = x;
  int y = 1;
  logic "add x x y";
  return r;
}
#endif

}  // namespace b2style

void main() {
  int i = 0;
  i++;
  string tmp;
  logic "interrupt putchar i tmp";
}
)B2STYLE_EOF";

            inline const std::string i__2 = R"B2STYLE_EOF(
#include <b2style.h>
#include <stdio.h>

void main() {
  int i = 0;
  i++;
  putchar(i);
  i++;
  putchar(i);
  i++;
  putchar(i);
  i++;
  putchar(i);
}
)B2STYLE_EOF";

            inline const std::string ifndef = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

#include <stdio.h>
#include "stdio.h"
#include <cstdio>
#include "cstdio"

void main () {
  b2style::std_out("good");
}
)B2STYLE_EOF";

            inline const std::string include = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

#include <cstdio>

int main() {
  int x = getchar();
  putchar(x);
  x = getchar();
  putchar(x);
  x = getchar();
  putchar(x);
  x = getchar();
  if (x == eof()) {
    b2style::std_out("EOF");
  }
}
)B2STYLE_EOF";

            inline const std::string include2 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

#include "cstdio"

int main() {
  int x = getchar();
  putchar(x);
  x = getchar();
  putchar(x);
  x = getchar();
  putchar(x);
  x = getchar();
  if (x == eof()) {
    b2style::std_out("EoF");
  }
}
)B2STYLE_EOF";

            inline const std::string legacy_biguint_to_str = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  biguint i = 4294967296L;
  biguint j = 4294967296l;
  biguint k = 1L;
  biguint l = 0L;
  b2style::std_out(b2style::legacy_biguint_to_str(i));
  b2style::std_out(b2style::legacy_biguint_to_str(j));
  b2style::std_out(b2style::legacy_biguint_to_str(k));
  b2style::std_out(b2style::legacy_biguint_to_str(l));
}
)B2STYLE_EOF";

            inline const std::string loaded_method = R"B2STYLE_EOF(
#include <b2style.h>
#include <bstyle/const.h>
#include <b2style/stdio.h>

long loaded_current_ms() {
  load_method("current_ms");
  long result;
  logic "interrupt execute_loaded_method @@prefixes@temps@string result";
  return result;
}

bool is_even(long i) {
  long _2 = 2;
  long _0 = 0;
  return b2style::equal(b2style::mod(i, _2), _0);
}

void main() {
  for (int i = 0; i < 100; i += 1) {
    long x = loaded_current_ms();
    if (is_even(x)) {
	  x = --x;
	}
    b2style::std_out(is_even(x));
  }
} 
)B2STYLE_EOF";

            inline const std::string lots_of_semi_colons = R"B2STYLE_EOF(
#include <b2style.h>;;; ;;;
#include <b2style/stdio.h>
// This test;;;
// tries to add unnecessary semi-colons ;;; to everywhere.

class C {
  ;
  int x ;;

  void print() {
    b2style::std_out(this.x);;
	;
  };;
};;;

void main() { ;;;
  C c;;
  c.x = 100; ;;;
  ;
  c.print();;;
};
)B2STYLE_EOF";

            inline const std::string multiline_string = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  b2style::std_out("a
    b
    c
    d");
}
)B2STYLE_EOF";

            inline const std::string namespaces = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

namespace a {
  namespace b {
    ::void f2() {
      ::b2style::std_out("a::b::f2\n");
    }
  }

  ::void f3() {
    ::b2style::std_out("a::f3\n");
  }

  namespace b {
    ::void f1() {
      ::b2style::std_out("a::b::f1\n");
    }
  }

  ::void f2() {
    ::b2style::std_out("a::f2\n");
  }

  namespace c {
    ::void f4() {
      ::b2style::std_out("a::c::f4\n");
    }
  }
}

::void main() {
  a::b::f1();
  a::f2();

  ::a::b::f2();
  ::a::f3();
  ::a::c::f4();
}

)B2STYLE_EOF";

            inline const std::string negative_int = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  b2style::std_out(-1);
  b2style::std_out(-2);
  b2style::std_out(-3);
}
)B2STYLE_EOF";

            inline const std::string nested_class = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class C1 {
  int x;

  void print() {
    b2style::std_out(this.x);
  }
};

class C2 {
  int x;
  C1 y;

  void print() {
    b2style::std_out(this.x);
	this.y.print();
  }
};

void main() {
  C2 c;
  c.x=100;
  c.y.x=200;
  c.print();
}
)B2STYLE_EOF";

            inline const std::string nested_heap_access = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  int a[10];
  for (int i = 0; i < 10; i++) {
    a[i] = i + 1;
  }

  b2style::std_out(a[a[0]]);
  b2style::std_out(a[a[a[0]]]);
  b2style::std_out(a[a[a[a[0]]]]);
  // value syntaxer matching is quite costly.
  /*
  b2style::std_out(a[a[a[a[a[0]]]]]);
  b2style::std_out(a[a[a[a[a[a[0]]]]]]);
  b2style::std_out(a[a[a[a[a[a[a[0]]]]]]]);
  */
}
)B2STYLE_EOF";

            inline const std::string nested_paragraph = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  {
    string x = "hello again again world";
    {
	  string x = "hello world\n";
	  b2style::std_out(x);
	}
	{
	  string x = "hello again world\n";
	  b2style::std_out(x);
	}
	b2style::std_out(x);
  }
}
)B2STYLE_EOF";

            inline const std::string nested_template = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class C {
  void print() {
    b2style::std_out("C");
  }
};

template <T>
class D {
  void print(T t) {
    t.print();
  }
};

template <T, T2>
class E {
  void print(T t, T2 t2) {
    t.print(t2);
  }
};

void main() {
  C c;
  D<C> d;
  E<D<C>, C> e;
  e.print(d, c);
}
)B2STYLE_EOF";

            inline const std::string order_of_operators = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  b2style::std_out(1 + 2 * 3 + 4 - 5 * 6 * 7 * 8 * 9 / 10 * (11 + 12));
}
)B2STYLE_EOF";

            inline const std::string pi_integral_0_1 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  ufloat dx = 0.1;
  ufloat x = 0.0;
  ufloat s = 0.0;
  while (x < 1.0) {
    ufloat c = 1.0 - (x ^ 2.0);
	c ^= 0.5;
	c *= dx;
	s += c;
	x += dx;
  }

  b2style::std_out(s * 4.0);
}
)B2STYLE_EOF";

            inline const std::string primitive_template = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

template <T>
class Increaser {
  T inc(T t) {
    return ++t;
  }
};

void main() {
  Increaser<int> i1;
  b2style::std_out(i1.inc(100));
  Increaser<ufloat> i2;
  b2style::std_out(i2.inc(100.11));
}
)B2STYLE_EOF";

            inline const std::string reinterpret_cast_ = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class S {
  int x;
  void print() {
    b2style::std_out(this.x);
  }
};

struct S2 {
  int x;
};

// This function should not be used.
void f(S& s) {
  s.x = 0;
}

void f(S2& s) {
  s.x = 100;
}

void main() {
  S s;
  s.x = -100;
  s.print();
  S2__struct__type__id__type s.S2__struct__type__id;
  // Note: this is not a typical way of using reinterpret_cast, it's pretty much useless without the support of
  // inheritance.
  reinterpret_cast(s, S2);
  f(s);
  reinterpret_cast(s, S);
  s.print();
}
)B2STYLE_EOF";

            inline const std::string reinterpret_cast_heap = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class S {
  int x;
  void print() {
    b2style::std_out(this.x);
  }
  void f() {
    this.x = 0;
  }
};

class S2 {
  int x;
  void f() {
	this.x = 10;
  }
};

void main() {
  S s[1];
  s[0].x = -10;
  s[0].print();
  S2__struct__type__id__type s.S2__struct__type__id[1];
  reinterpret_cast(s, S2);
  s[0].f();
  reinterpret_cast(s, S);
  s[0].print();
}
)B2STYLE_EOF";

            inline const std::string reinterpret_cast_to_a_different_class_type = R"B2STYLE_EOF(
#include <b2style.h>

class S {
  int x;
};

struct S2 {
  int x;
};

void main() {
  S s;
  S2__struct__type__id__type s.S2__struct__type__id; 
  reinterpret_cast(s, S2);
  undefine(s.S__struct__type__id);
  // On b3style, it will also complain the destruction of s as S2 cannot be generated.
  // b2style would only complain the S2__struct__type__id hasn't been defined.
}
)B2STYLE_EOF";

            inline const std::string self_add = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  int i = 100;
  i += 1;
  b2style::std_out(i);
  i = i++;
  b2style::std_out(i);
  i = ++i;
  b2style::std_out(i);
}
)B2STYLE_EOF";

            inline const std::string shift = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  int i = 100;
  i <<= 2;
  b2style::std_out(i);
  int j = 0;
  j = i >> 5;
  b2style::std_out(i);
  b2style::std_out(j);
}
)B2STYLE_EOF";

            inline const std::string str_unescape = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  b2style::std_out("abc\tdef\n");
}
)B2STYLE_EOF";

            inline const std::string struct_function_ref = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

struct S {
  int x;
  string y;
};

void f(S& s) {
  s.x=100;
  s.y="abc";
}

void main() {
  S s;
  f(s);
  b2style::std_out(s.x);
  b2style::std_out(s.y);
}
)B2STYLE_EOF";

            inline const std::string struct_in_namespace = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

namespace N {
  typedef ::string string;
  struct S {
    string x;
  };

  struct T {
    S s;
  };
}  // namespace N

void main() {
  N::T t;
  t.s.x = "100";
  b2style::std_out(t.s.x);
}
)B2STYLE_EOF";

            inline const std::string template_ = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

class D {
  string to_string() {
    return "D";
  }
};

class E {
  string to_string() {
    return "E";
  }
};

template <T>
class C {
  string to_string(T t) {
    return t.to_string();
  }
};

// TODO: Allow class D and class E to be defined after template<T> C.

void main() {
  C<D> c;
  D d;
  b2style::std_out(c.to_string(d));
  C<E> c2;
  E e;
  b2style::std_out(c2.to_string(e));
}
)B2STYLE_EOF";

            inline const std::string template_template_case1 = R"B2STYLE_EOF(
template <T>
class C {
  T x;
  void f(T y) {}
};)B2STYLE_EOF";

            inline const std::string template_with_different_length = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

template <T>
class C {
  T t;
};

template <T, T2>
class C {
  T t;
  T2 t2;
};

void main() {
  C<int> c;
  C<int, string> c2;
  c.t = 1;
  c2.t = 2;
  c2.t2 = "abc";

  b2style::std_out(c.t);
  b2style::std_out(c2.t);
  b2style::std_out(c2.t2);
}
)B2STYLE_EOF";

            inline const std::string template_wont_be_extended_twice = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

template <T>
class Adder {
  T add(T t, T t2) {
    return t + t2;
  }
};

void main() {
  Adder<int> t;
  Adder<int> t2;
  b2style::std_out(t.add(1, 1));
  b2style::std_out(t2.add(2, 2));
}
)B2STYLE_EOF";

            inline const std::string test_assert = R"B2STYLE_EOF(
#include <b2style.h>
#include <testing.h>

void main() {
  b2style::testing::assert_true(false);
  b2style::testing::assert_true(true);
  b2style::testing::finished();
})B2STYLE_EOF";

            inline const std::string two_template_type_parameters = R"B2STYLE_EOF(
template<T, T2>
class C {
  T x;
  T2 y;
  void p(T x, T2 y) {}
};)B2STYLE_EOF";

            inline const std::string typedef_ = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

namespace a {
  typedef ::void void;
  void f() {
    ::b2style::std_out("abc");
  }
}  // namespace a

void main() {
  a::f();
}
)B2STYLE_EOF";

            inline const std::string ufloat_operators = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  ufloat i = 2.2;
  i += 3.0;
  i -= 3.0;
  i /= 2.0;
  b2style::std_out(i);
  i = b2style::ufloat::fraction(1, 2);
  b2style::std_out(i);
}
)B2STYLE_EOF";

            inline const std::string ufloat_std_out = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  ufloat f = 1.1;
  b2style::std_out(f);
}
)B2STYLE_EOF";

            inline const std::string vector_destructor = R"B2STYLE_EOF(
#include <b2style.h>
#include <std/vector>

void main() {
  std::vector<int> v();
  for (int i = 0; i < 10; i++) {
    v.push_back(i + 1);
  }
}
)B2STYLE_EOF";

            inline const std::string while_0_to_1 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  ufloat x = 0.0;
  ufloat i = 0.0;
  while (i < 1.0) {
    i += 0.01;
	x += i;
  }
  b2style::std_out(x);
  b2style::std_out("\n");
}
)B2STYLE_EOF";

            inline const std::string while_1_to_100 = R"B2STYLE_EOF(
#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  int x = 0;
  int i = 0;
  while (i < 100) {
    i += 1;
	x += i;
  }
  b2style::std_out(x);
  b2style::std_out("\n");
}
)B2STYLE_EOF";


            inline const std::string errors_class_initializer_for_non_class = R"B2STYLE_EOF(

#include <b2style.h>

logic "type ooops_this_is_not_a_class 1";

void main() {
  ooops_this_is_not_a_class ooops_this_is_not_a_class_value(10);
})B2STYLE_EOF";

            inline const std::string errors_cycle_typedef = R"B2STYLE_EOF(

#include <b2style.h>

namespace CYCLE_TYPEDEF {
  typedef A B;
  typedef B C;
  typedef C A;
}  // namespace CYCLE_TYPEDEF

void main() {
  CYCLE_TYPEDEF::A a;
})B2STYLE_EOF";

            inline const std::string errors_dollar_in_number = R"B2STYLE_EOF(

void main() {
  float x = 1$1;
})B2STYLE_EOF";

            inline const std::string errors_duplicate_template_type_parameters = R"B2STYLE_EOF(

template<T, T>
class C{};)B2STYLE_EOF";

            inline const std::string errors_function_name_ends_with_dot = R"B2STYLE_EOF(

class C{
  int x;

  void print() {}
};

void main() {
  C c;
  c.();
})B2STYLE_EOF";

            inline const std::string errors_function_return_struct_type_mismatch = R"B2STYLE_EOF(

#include <b2style.h>

struct S1 {
  int x;
  string y;
};

struct S2 {
  int v;
  string w;
};

S2 f() {
  S1 s;
  s.x = 100;
  s.y = "abc";
  return s;
}

void main() {
  S2 s = f();
}
)B2STYLE_EOF";

            inline const std::string errors_include_needs_wraps = R"B2STYLE_EOF(

#include abc.h

void main() {})B2STYLE_EOF";

            inline const std::string errors_missing_ending_quota = R"B2STYLE_EOF(

#include <b2style.h>
#include <b2style/stdio.h>

void main() {
  b2style::std_out("abc);
}
)B2STYLE_EOF";

            inline const std::string errors_reinterpret_cast_heap_with_index = R"B2STYLE_EOF(

#include <b2style.h>

struct S {
  int x;
};

struct S2 {
  int x;
};

void main() {
  S s[1];
  S2__struct__type__id__type s.S2__struct__type__id[1];
  reinterpret_cast(s[0], S2);
}
)B2STYLE_EOF";

            inline const std::string errors_reinterpret_cast_unknown_type = R"B2STYLE_EOF(

#include <b2style.h>

void f(int x) {}

void main() {
  int x;
  reinterpret_cast(x, this_is_an_unknown_type);
  f(x);
}
)B2STYLE_EOF";

            inline const std::string errors_reinterpret_cast_unknown_variable = R"B2STYLE_EOF(

#include <b2style.h>

void main() {
  reinterpret_cast(this_is_an_unknown_variable, int);
})B2STYLE_EOF";

            inline const std::string errors_reinterpret_cast_without_type_id = R"B2STYLE_EOF(

#include <b2style.h>
#include <b2style/stdio.h>

class S {
  int x;
  void print() {
    b2style::std_out(this.x);
  }
};

struct S2 {
  int x;
};

// This function should not be used.
void f(S& s) {
  s.x = 0;
}

void f(S2& s) {
  s.x = 100;
}

void main() {
  S s;
  s.x = -100;
  s.print();
  reinterpret_cast(s, S2);
  f(s);  // s.S2__struct__type__id should be missing here.
  reinterpret_cast(s, S);
  s.print();
}
)B2STYLE_EOF";

            inline const std::string errors_template_without_type_parameter = R"B2STYLE_EOF(

#include <b2style.h>

template<>
class ThisTemplateHasNoTypeParameter {};

void main() {})B2STYLE_EOF";

            inline const std::string errors_three_pluses = R"B2STYLE_EOF(

#include   <b2style.h>

void main() {
  int i = 0;
  i +++;
}
)B2STYLE_EOF";

            inline const std::string errors_undefined_value_clause = R"B2STYLE_EOF(

void main() {
  this_is_an_undefined_value_clause = 100;
})B2STYLE_EOF";

            inline const std::string errors_value_clause_struct_type_mismatch = R"B2STYLE_EOF(

#include <b2style.h>

struct S1 {
  int x;
  string y;
};

struct S2 {
  int v;
  string w;
};

S1 f() {
  S1 s;
  s.x = 100;
  s.y = "abc";
  return s;
}

void main() {
  S2 s = f();
})B2STYLE_EOF";

            inline const std::string errors_value_clause_struct_type_mismatch2 = R"B2STYLE_EOF(

#include <b2style.h>

struct S1 {
  int x;
  string y;
};

struct S2 {
  int v;
  string w;
};

S1 f() {
  S1 s;
  s.x = 100;
  s.y = "abc";
  return s;
}

void main() {
  S1 s1 = f();
  S2 s2 = s1;
})B2STYLE_EOF";

        }
    }
}
