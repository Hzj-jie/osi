#pragma once
#include <string>

namespace logic_test
{
    namespace import_cases
    {
        inline const std::string case1 = R"(
## define a type uint of 4 bytes
type uint 4
## define a type bool of 1 byte
type bool 1

callee copy-one-byte2 type* ( source type* target type* pos uint len uint ) {
    define char type*
    cut_len char source pos len
    append target char
    return copy-one-byte2 target
}

define source type*
define target1 type*
define target2 type*

callee copy-one-byte1 type0 ( source type* pos uint len uint ) {
    define char type*
    cut_len char source pos len
    append target1 char
    return copy-one-byte1 *
}

## copy a const string to source
copy_const source Ehello\x20world\n

define 0 uint
copy_const 0 i0
define 1 uint
copy_const 1 i1

define length uint
sizeof length source

define i uint
copy i length
define not-finished bool
more not-finished i 0
while_then not-finished {
    subtract i i 1
    caller copy-one-byte1 ( source i 1 )
    more not-finished i 0
}

copy i length
more not-finished i 0
while_then not-finished {
    subtract i i 1
    define result type*
    caller copy-one-byte2 result ( source target2 i 1 )
    move target2 result
    more not-finished i 0
}
)";

        inline const std::string case2 = R"(
## Use a stupid way to calculate n! from 1 to 100 and push the results into a
## slice buffer.

define n! type*

type uint 4
type bool 1

define 0 uint
copy_const 0 i0
define 1 uint
copy_const 1 i1
define 2 uint
copy_const 2 i2
define 3 uint
copy_const 3 i3

callee method1 type0 ( n type* ) {
  define result type*
  copy_const result i1
  define i type*
  copy_const i i0
  define not-continue bool
  equal not-continue i n
  do_until not-continue {
    add i i 1
    multiply result result i
    equal not-continue i n
  }
  append_slice n! result
}

callee method2 type0 ( n type* ) {
  define result type*
  copy_const result i1
  define i type*
  copy_const i i0
  define continue bool
  less continue i n
  do_while continue {
    add i i 1
    multiply result result i
    less continue i n
  }
  append_slice n! result
}

callee method3-impl type* ( n type* ) {
  define result type*
  copy_const result i1
  define i type*
  copy_const i i2
  define continue bool
  less_or_equal continue i n
  while_then continue {
    multiply result result i
    add i i 1
    less_or_equal continue i n
  }
  return method3-impl result
}

callee method3 type0 ( n type* ) {
  define result type*
  caller method3-impl result ( n )
  append_slice n! result 
}

define 100 uint
copy_const 100 i100
define i type*
copy_const i i1
define continue bool
less_or_equal continue i 100
while_then continue {
  define rnd type*
  interrupt current_ms 1 rnd
  define result type*
  divide result rnd rnd 3

  define choose bool
  equal choose rnd 0
  if choose {
    caller method1 ( i )
  }
  equal choose rnd 1
  if choose {
    caller method2 ( i )
  }
  equal choose rnd 2
  if choose {
    caller method3 ( i )
  }

  add i i 1
  less_or_equal continue i 100
}
)";

        inline const std::string case3 = R"(
define output type*

type uint 4
type bool 1

define 0 uint
copy_const 0 i0
define 1 uint
copy_const 1 i1
define 2 uint
copy_const 2 i2
define 3 uint
copy_const 3 i3

callee method1 type0 ( n type* ) {
  append_slice output n
}

callee method2 type0 ( n type* ) {
  append_slice output n
}

callee method3 type0 ( n type* ) {
  append_slice output n
}

define 100 uint
copy_const 100 i100
define i type*
copy_const i i1
define continue bool
less_or_equal continue i 100
while_then continue {
  define rnd type*
  interrupt current_ms 1 rnd
  define result type*
  divide result rnd rnd 3

  define choose bool
  equal choose rnd 0
  if choose {
    caller method1 ( i )
  }
  equal choose rnd 1
  if choose {
    caller method2 ( i )
  }
  equal choose rnd 2
  if choose {
    caller method3 ( i )
  }

  add i i 1
  less_or_equal continue i 100
}
)";

        inline const std::string case4 = R"(
define temp_string type*

callee main type0 ( ) {
  define result type*
  copy_const result Ehello\x20world
  interrupt stdout result temp_string
  interrupt stderr result temp_string
}

caller main ( )
)";

        inline const std::string heap = R"(
type uint 4
type bool 1

define true bool
copy_const true b1

## Always use a new scope to ensure the heap allocation can be freed.
if true {

    define uint_100 uint
    copy_const uint_100 i100
    define_heap h uint uint_100
    define uint_99 uint
    copy_const uint_99 i99
    copy h[uint_99] uint_99
    define out uint
    copy out h[uint_99]

    define temp_string type*
    interrupt putchar out temp_string

}
)";

        inline const std::string address_of = R"(
type uint 4
define 1 uint
copy_const 1 i1

callee m uint ( n uint ) {
  add n n 1
  return m n
}

callee m2 uint ( n uint ) {
  subtract n n 1
  return m2 n
}

callee_ref delegate uint ( uint )
define f delegate
define n uint
define temp_string type*

address_of f m
caller_ref f n ( 1 ) 
interrupt stdout n temp_string

address_of f m2
caller_ref f n ( n )
interrupt stdout n temp_string
)";

        inline const std::string address_of_address_of_param = R"(
type uint 4
define 1 uint
copy_const 1 i1

callee m type0 ( ) {
  define temp_string type*
  interrupt stdout 1 temp_string
}

callee_ref delegate type0 ( )

callee m2 type0 ( f delegate ) {
  caller_ref f ( )
}

callee_ref delegate2 type0 ( delegate )

define f delegate2
address_of f m2
define g delegate
address_of g m
caller_ref f ( g )
)";

        inline const std::string address_of_param = R"(
type uint 4
define 1 uint
copy_const 1 i1

callee m uint ( n uint ) {
  add n n 1
  return m n
}

callee m2 uint ( n uint ) {
  subtract n n 1
  return m2 n
}

callee_ref delegate uint ( uint )
define n uint
copy_const n i1
define temp_string type*

callee run type0 ( f delegate ) {
  caller_ref f n ( n )
  interrupt stdout n temp_string
}

define f delegate
address_of f m
caller run ( f )
address_of f m2
caller run ( f )
)";

        inline const std::string address_of_ref = R"(
type uint 4
define 1 uint
copy_const 1 i1

callee m type0 ( n uint& ) {
  add n n 1
  return m *
}

callee m2 type0 ( n uint& ) {
  subtract n n 1
  return m2 *
}

callee_ref delegate uint ( uint& )
define f delegate
define n uint
define temp_string type*
copy_const n i1

address_of f m
caller_ref f ( n ) 
interrupt stdout n temp_string

address_of f m2
caller_ref f ( n )
interrupt stdout n temp_string
)";

        inline const std::string copy_address_of = R"(
type uint 4
define 1 uint
copy_const 1 i1

callee m uint ( n uint ) {
  add n n 1
  return m n
}

callee_ref delegate uint ( uint )
define f delegate
define n uint
define temp_string type*

address_of f m
define g delegate
copy g f
caller_ref g n ( 1 ) 
interrupt stdout n temp_string
)";

        inline const std::string callee_ref = R"(
callee m type0 ( n type*& i type* ) {
  append n i
}

callee m2 type0 ( n type*& ) {
  define 1 type*
  copy_const 1 i1
  add n n 1
}

define n type*
define i type*
copy_const i sd

caller m ( n i )
caller m ( n i )
caller m ( n i )
caller m ( n i )

define temp_string type*
interrupt stdout n temp_string

define v type*
copy_const v i0

caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )
caller m2 ( v )

interrupt putchar v temp_string
)";

        inline const std::string callee_ref2 = R"(
type Integer 4

callee b2style__int_std_out&Integer type0 ( x Integer ) {
  define temp type*
  interrupt putchar x temp
}

callee b2style__add&Integer&Integer Integer ( b2style__i Integer b2style__j Integer ) {
  add b2style__i b2style__i b2style__j
  return b2style__add&Integer&Integer b2style__i
}

callee self_increment&Integer Integer ( x Integer& ) {
  define temp_value_@1@34417-34418 Integer
  copy_const temp_value_@1@34417-34418 a01000000
  define temp_value_@1@34398-34420 Integer
  caller b2style__add&Integer&Integer temp_value_@1@34398-34420 ( x temp_value_@1@34417-34418 )
  move x temp_value_@1@34398-34420
  return self_increment&Integer x
}

callee main type0 ( ) {
  define x Integer
  define temp_value_@1@34460-34461 Integer
  copy_const temp_value_@1@34460-34461 a00000000
  move x temp_value_@1@34460-34461
  define temp_value_@1@34487-34507 Integer
  caller self_increment&Integer temp_value_@1@34487-34507 ( x )
  caller b2style__int_std_out&Integer ( temp_value_@1@34487-34507 )
  define temp_value_@1@34535-34555 Integer
  caller self_increment&Integer temp_value_@1@34535-34555 ( x )
  caller b2style__int_std_out&Integer ( temp_value_@1@34535-34555 )
  define temp_value_@1@34583-34603 Integer
  caller self_increment&Integer temp_value_@1@34583-34603 ( x )
  caller b2style__int_std_out&Integer ( temp_value_@1@34583-34603 )
  define temp_value_@1@34631-34651 Integer
  caller self_increment&Integer temp_value_@1@34631-34651 ( x )
  caller b2style__int_std_out&Integer ( temp_value_@1@34631-34651 )
}

caller main ( )
)";
    }
}
