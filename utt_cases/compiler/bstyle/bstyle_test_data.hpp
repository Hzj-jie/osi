#pragma once
#include <string>

namespace osi
{
    namespace compiler
    {
        namespace bstyle_test_data
        {
            inline const std::string case1 = R"(
#include <bstyle.h>

void print(string i) {
  string temp;
  logic "interrupt stdout i temp";
}

void main() {
  print("Hello World");
}
)";

            inline const std::string case2 = R"(
#include <bstyle.h>

bool equal(int i, int j) {
  bool result;
  logic "equal result i j";
  return result;
}

bool is_even(long i) {
  int quotient;
  int remainder;
  int v = 2;
  logic "divide quotient remainder i v";
  return equal(remainder, 0);
}

void print(bool i) {
  string temp;
  string _true = "True";
  string _false = "False";
  if (i) {
	logic "interrupt stdout _true temp";
  } else {
	logic "interrupt stdout _false temp";
  }
}

long current_ms() {
  long result;
  int temp;
  logic "interrupt current_ms temp result";
  return result;
}

long self_decrease(long x) {
  int temp = 1;
  logic "subtract x x temp";
  return x;
}

int self_increase(int x) {
  int temp = 1;
  logic "add x x temp";
  return x;
}

bool less(int x, int y) {
  bool result;
  logic "less result x y";
  return result;
}

void main() {
  for (int i = 0; less(i, 100); i = self_increase(i)) {
    long x;
    x = current_ms();
    if (is_even(x)) {
      x = self_decrease(x);
    }
    print(is_even(x));
  }
}
)";

            inline const std::string global_variable = R"(
#include <bstyle.h>

string _true = "True";
string _false = "False";

void print(bool i) {
  string temp;
  if (i) {
	logic "interrupt stdout _true temp";
  } else {
	logic "interrupt stdout _false temp";
  }
}

void main() {
  print(true);
  print(false);
}
)";

            inline const std::string overload_function = R"(
#include <bstyle.h>

biguint to_biguint(int i) {
  return i;
}

biguint to_biguint(long i) {
  return i;
}

long to_long(int i) {
  return i;
}

bool equal(biguint i, biguint j) {
  bool result;
  logic "equal result i j";
  return result;
}

bool not(bool i) {
  if (i) return false;
  return true;
}

bool equal(int i, int j) {
  return equal(to_biguint(i), to_biguint(j));
}

bool equal(long i, long j) {
  return not(equal(to_biguint(i), to_biguint(j)));
}

void print(bool i) {
  string temp;
  string _true = "True";
  string _false = "False";
  if (i) {
	logic "interrupt stdout _true temp";
  } else {
	logic "interrupt stdout _false temp";
  }
}

void main() {
  print(equal(100, 100));
  print(equal(100, 200));
  print(equal(to_long(100), to_long(100)));
  print(equal(to_long(100), to_long(200)));
}
)";

            inline const std::string single_level_struct = R"(
#include <bstyle.h>
#include <stdio.h>

struct s {
  int i;
  string s;
};

void main() {
  s x;
  x.i = 100;
  x.s = "abc";
  putchar(x.i);
  logic "interrupt stdout x.s @@prefixes@temps@string";
}
)";

            inline const std::string nested_struct = R"(
#include <bstyle.h>
#include <stdio.h>

struct S1 {
  int x;
};

struct S2 {
  S1 x;
  int y;
};

void print(S1 s) {
  putchar(s.x);
}

void print(S2 s) {
  print(s.x);
  putchar(s.y);
}

void main() {
  S2 s;
  s.x.x = 100;
  s.y = 100;
  print(s);
}
)";

            inline const std::string function_struct = R"(
#include <bstyle.h>

struct S {
  string s;
  string s2;
};

void f(S s) {
  string temp;
  logic "interrupt stdout s.s temp";
  logic "interrupt stdout s.s2 temp";
}

void g(S s) {
  f(s);
}

void main() {
  S s;
  s.s = "abc";
  s.s2 = "def";
  g(s);
}
)";

            inline const std::string return_struct = R"(
#include <bstyle.h>

struct S {
  string s;
  string s2;
};

S f() {
  S s;
  s.s = "abc";
  s.s2 = "def";
  return s;
}

void main() {
  S s = f();
  string temp;
  logic "interrupt stdout s.s temp";
  logic "interrupt stdout s.s2 temp";
}
)";

            inline const std::string call_struct_on_heap = R"(
struct S {
  String s;
};

type0 f(S s) {
  String temp;
  logic "interrupt stdout s.s temp";
}

type0 f(Integer i) {
  String temp;
  logic "interrupt putchar i temp";
}

type0 main() {
  S s[1];
  // TODO: Make s[0].s work.
  s.s[0] = "abc";
  f(s[0]);
  Integer v[1];
  v[0] = 100;
  f(v[0]);
}
)";

            inline const std::string for_loop = R"(
typedef Integer int;
typedef Boolean bool;
typedef type0 void;
typedef String string;

void f(int& x) {
  int y = 1;
  logic "add x x y";
}

bool less(int x, int y) {
  bool result;
  logic "less result x y";
  return result;
}
 
void main() {
  int r;
  for (int i = 0; less(i, 5); f(i)) {
    logic "add r r i";
  }
  string temp;
  logic "interrupt putchar r temp";
}
)";

            inline const std::string empty_struct_overloads = R"(
typedef "type*" string;
typedef "type0" void;
typedef "type0" b2style__void;
typedef "type*" b2style__string;
logic "define @@prefixes@temps@string type*";

b2style__void b2style__std_out ( b2style__string b2style__i ) { logic "interrupt stdout b2style__i @@prefixes@temps@string" ;
}

 struct D { }
;
 struct E { }
;
 string to_string ( D & this ) { return "D" ;
 }
 string to_string ( E & this ) { return "E" ;
 }
struct C__D { }
;
 string to_string ( C__D & this ,  D t ) { return to_string ( t ) ;
 }
 struct C__E { }
;
 string to_string ( C__E & this ,  E t ) { return to_string ( t ) ;
 }
 void main ( ) { C__D c ;
 D d ;
 b2style__std_out ( to_string ( c ,  d ) ) ;
 C__E c2 ;
 E e ;
 b2style__std_out ( to_string ( c2 ,  e ) ) ;
 }
)";

            inline const std::string delegate_str = R"(
#include <bstyle.h>
#include <stdio.h>

delegate void func();

void f() {
  putchar(102);
}

void g() {
  putchar(103);
}

void main() {
  func x;
  x = f;
  x();
  x = g;
  x();
}
)";

            inline const std::string statement = R"(
#include <bstyle.h>

void std_out(string i) {
  string tmp;
  logic "interrupt stdout i tmp";
}

void main() {
  std_out(__STATEMENT__);
  string s = __STATEMENT__;
  std_out(s);
}
)";

            inline const std::string real__file__ = R"(
#include <bstyle.h>

void print(string i) {
  string temp;
  logic "interrupt stdout i temp";
}

void main() {
  print(__FILE__);
}
)";

            inline const std::string predefined_def = R"(
#include <bstyle.h>

#ifdef BSTYLE
void main() {}
#endif
)";

            inline const std::string func_name_with_dot = R"(
#include <bstyle.h>
#include <stdio.h>

void f.f(int x) {
  putchar(x);
}

void main() {
  f.f(1);
  f.f(2);
}
)";

            inline const std::string errors_dot_is_disallowed_as_the_end_of_name = R"(
void main() {
  int x.;
}
)";
        }
    }
}
