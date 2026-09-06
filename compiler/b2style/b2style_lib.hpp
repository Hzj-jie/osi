#pragma once
#include <string>
#include <unordered_map>

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            class b2style_lib
            {
            public:
                static const std::string& nlexer_rule()
                {
                    static const std::string rule = R"(

kw-if if
kw-else else
kw-for for
kw-while while
kw-do do
kw-loop loop
kw-return return
kw-break break
kw-logic logic
kw-include #include
kw-ifdef #ifdef
kw-delse #else
kw-ifndef #ifndef
kw-define #define
kw-endif #endif
kw-typedef typedef
kw-struct struct
kw-reinterpret-cast reinterpret_cast
kw-delegate delegate
kw-undefine undefine
kw-dealloc dealloc
kw-file __FILE__
kw-func __func__
kw-line __LINE__
kw-statement __STATEMENT__
kw-static-cast static_cast

single-line-comment //[*|\n]*
multi-line-comment /\*[*|\*/]*\*/

blank [\b]+

include-with-file #include[\b]+<[*|>]+>

bool [true,false]
integer [+,-]?[\d]+
biguint [\d]+[l,L]
ufloat [\d]*.[\d]+
string "[\",*|"]*"

comma \,
colon :
question-mark ?
start-paragraph {
end-paragraph }
start-bracket (
end-bracket )
start-square-bracket \[
end-square-bracket \]
semi-colon ;
assignment =
or \|\|

dot .


kw-namespace namespace
kw-class class
kw-template template
kw-overridable overridable
kw-override override

# Comparison
less-or-equal <=
greater-or-equal >=
equal ==
not-equal !=
less-than <
greater-than >

# Operator
add \+
minus -
multiply \*
divide /
mod %
power ^
bit-and &
bit-or \|
and &&
not !
self-inc \+\+
self-dec --
left-shift <<
# right-shift is conflict with template <T<T2>>
# right-shift >>

self-add \+=
self-minus -=
self-multiply \*=
self-divide /=
self-mod %=
self-power ^=
self-bit-and &=
self-bit-or \|=
self-and &&=
self-or \|\|=
self-left-shift <<=
self-right-shift >>=

double-colon ::
# "void :: f" is not allowed, it should be "void ::f", otherwise it's conflict with "void::f".
raw-name-with-double-colon [\w,_][\w,\d,_]*::


raw-name [\w,_][\w,\d,_]*
)";
                    return rule;
                }

                static const std::string& syntaxer_rule()
                {
                    static const std::string rule = R"(

IGNORE_TYPES blank, single-line-comment, multi-line-comment
ROOT_TYPES root-type

function type-name name start-bracket [paramlist]? end-bracket multi-sentence-paragraph
param paramtype name
paramlist [param-with-comma]* param
param-with-comma param comma

paragraph [sentence, multi-sentence-paragraph]
sentence [sentence-with-semi-colon, condition, while, for-loop]

multi-sentence-paragraph start-paragraph [paragraph, ifdef-wrapped, ifndef-wrapped]* end-paragraph

value-definition type-name name assignment value
value-declaration type-name name
heap-declaration type-name heap-name
value-clause variable-name assignment value

condition kw-if start-bracket value end-bracket paragraph [else-condition]?
else-condition kw-else paragraph

while kw-while start-bracket value end-bracket paragraph

base-for-increase [value-clause, ignore-result-function-call]
for-loop kw-for start-bracket [value-definition, value-declaration]? semi-colon [value]? semi-colon [for-increase]? end-bracket paragraph 

value [value-with-bracket, value-without-bracket]
base-value-without-bracket [function-call, variable-name, integer, biguint, ufloat, bool, string, kw-file, kw-func, kw-line, kw-statement]
value-with-bracket start-bracket value end-bracket

heap-name name start-square-bracket value end-square-bracket

ignore-result-function-call function-call

function-call name start-bracket [value-list]? end-bracket
value-list [value-with-comma]* value
value-with-comma value comma

return-clause kw-return [value]?

# Embed a logic statement directly, like logic "int stdout a_string"
logic kw-logic string

include-with-string kw-include string
include [include-with-string, include-with-file]

define-with-separator raw-name or
defines [define-with-separator]* raw-name
delse-wrapped kw-delse [root-type, paragraph]*
ifdef-wrapped kw-ifdef defines [root-type, paragraph]* [delse-wrapped]? kw-endif
ifndef-wrapped kw-ifndef defines [root-type, paragraph]* [delse-wrapped]? kw-endif
define kw-define raw-name

# Allow type* to be used as a string.
typedef-type-str string
typedef-type-name type-name
typedef-type [typedef-type-name, typedef-type-str]
typedef kw-typedef typedef-type typedef-type

# TODO: Support value-definition
struct-body [value-declaration]? semi-colon
struct kw-struct type-name start-paragraph [struct-body]* end-paragraph semi-colon

# Function-like instructions.
reinterpret-cast kw-reinterpret-cast start-bracket variable-name comma type-name end-bracket
undefine kw-undefine start-bracket variable-name end-bracket
dealloc kw-dealloc start-bracket variable-name end-bracket
static-cast kw-static-cast start-bracket variable-name comma type-name end-bracket

root-type-with-semi-colon [value-definition, heap-declaration, value-declaration, logic, typedef, delegate]? semi-colon
base-root-type [root-type-with-semi-colon, function, include, define, ifdef-wrapped, ifndef-wrapped, struct]
base-sentence-with-semi-colon [ignore-result-function-call, value-definition, heap-declaration, value-declaration, value-clause, return-clause, kw-break, logic, typedef, reinterpret-cast, delegate, undefine, dealloc, static-cast]? semi-colon

delegate kw-delegate type-name name start-bracket [paramtypelist]? end-bracket
paramtype type-name [reference]?
paramtypelist [paramtype-with-comma]* paramtype
paramtype-with-comma paramtype comma

root-type [base-root-type, namespace, class, template]

right-shift greater-than greater-than
raw-name-with-dot raw-name dot
# Support a.N::b function call and a.::N::b.
# Otherwise it should be name [double-colon]? [raw-name-with-double-colon]* [raw-name-with-dot]* raw-name
name [raw-name-with-dot]* [double-colon]? [raw-name-with-double-colon, raw-name-with-dot]* raw-name
raw-type-name [double-colon]? [raw-name-with-double-colon]* raw-name

type-name [template-type-name, raw-type-name]

reference bit-and

namespace kw-namespace name start-paragraph [root-type]* end-paragraph

# Can easily share logic with existing code.
function-name-with-template name less-than paramtypelist greater-than
function-call-with-template function-name-with-template start-bracket [value-list]? end-bracket
ignore-result-function-call-with-template function-call-with-template
class-initializer type-name name start-bracket [value-list]? end-bracket
b2style-sentence-with-semi-colon [ignore-result-heap-struct-function-call, ignore-result-function-call-with-template, self-value-clause, pre-operation-value, post-operation-value, class-initializer]? semi-colon
sentence-with-semi-colon [b2style-sentence-with-semi-colon, base-sentence-with-semi-colon]

self-value-clause variable-name [self-add, self-minus, self-multiply, self-divide, self-mod, self-power, self-bit-and, self-bit-or, self-and, self-or, self-left-shift, self-right-shift] value

for-increase [self-value-clause, ignore-result-heap-struct-function-call, pre-operation-value, post-operation-value, base-for-increase]

raw-value [heap-struct-function-call, function-call-with-template, base-value-without-bracket]
value-without-bracket [value-with-operation, raw-value]
value-with-operation [unary-operation-value, binary-operation-value]
unary-operation-value [pre-operation-value, post-operation-value]
pre-operation-value [not, self-inc, self-dec] value
# Should support post-operation-value value [self-inc, self-dec]
post-operation-value raw-value [self-inc, self-dec]
# Should support binary-operation-value value [add, minus, multiply, divide, mod, power, bit-and, bit-or, and, or, less-than, greater-than, less-or-equal, greater-or-equal, equal, not-equal] value
binary-operation-value [value-with-bracket, raw-value] [add, minus, multiply, divide, mod, power, bit-and, bit-or, and, or, left-shift, right-shift, less-than, greater-than, less-or-equal, greater-or-equal, equal, not-equal] value
variable-name [heap-struct-name, heap-name, name]
heap-struct-name heap-name dot name
ignore-result-heap-struct-function-call heap-struct-function-call
heap-struct-function-call heap-struct-name start-bracket [value-list]? end-bracket

# TODO: Support static function
type-name-with-comma type-name comma
inherited-types [type-name-with-comma]* type-name
class-inheritance colon inherited-types
overridable-function kw-overridable function
override-function kw-override function
class-function [overridable-function, override-function, function]
class-template-function template-head class-function
class kw-class type-name [class-inheritance]? start-paragraph [struct-body, class-function, class-template-function]* end-paragraph semi-colon

template-type-name raw-type-name less-than paramtypelist greater-than
type-param-list [type-param-with-comma]* type-param
type-param-with-comma type-param comma
type-param raw-name
delegate-with-semi-colon delegate semi-colon
template-body [class, delegate-with-semi-colon, function]
template-head kw-template less-than type-param-list greater-than
template template-head template-body
)";
                    return rule;
                }

                static const std::unordered_map<std::string, std::string>& files()
                {
                    static const std::unordered_map<std::string, std::string> m = {
                        {"assert.h", R"B2HDR(

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

#endif  // B2STYLE_LIB_ASSERT_H)B2HDR"},
                        {"b2style.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_H
#define B2STYLE_LIB_B2STYLE_H

#include <bstyle.h>
#include <b2style/operators.h>
#include <b2style/ufloat.h>

#endif  // B2STYLE_LIB_B2STYLE_H
)B2HDR"},
                        {"b2style/delegates.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_DELEGATES_H
#define B2STYLE_LIB_B2STYLE_DELEGATES_H

#include <b2style/types.h>

namespace b2style {

template <RT>
delegate RT function();

template <T, RT>
delegate RT function(T);

template <T, T2, RT>
delegate RT function(T, T2);

template <T, T2, T3, RT>
delegate RT function(T, T2, T3);

template <T, T2, T3, T4, RT>
delegate RT function(T, T2, T3, T4);

template <T, T2, T3, T4, T5, RT>
delegate RT function(T, T2, T3, T4, T5);

template <T, T2, T3, T4, T5, T6, RT>
delegate RT function(T, T2, T3, T4, T5, T6);

template <T, T2, T3, T4, T5, T6, T7, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7);

template <T, T2, T3, T4, T5, T6, T7, T8, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16);

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_DELEGATES_H)B2HDR"},
                        {"b2style/heap_ptr.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_HEAP_PTR_H
#define B2STYLE_LIB_B2STYLE_HEAP_PTR_H

#include <b2style/types.h>
#include <assert.h>

namespace b2style {

// Type T is required to use the dealloc instruction.
template <T>
class heap_ptr {
  T _a;
  int _s;

  int size() {
    return this._s;
  }

  bool empty() {
    return this.size() == 0;
  }

  void construct() {
    this._s = 0;
  }

  void destruct() {
    if (this.empty()) return;
    dealloc(this._a);
    this.construct();
  }

  void alloc(int size) {
    this.destruct();
    ::assert(size > 0);
    T x[size];
    this._a = x;
    this._s = size;
    undefine(x);
  }

  void construct(int size) {
    this.construct();
    this.alloc(size);
  }

  T get(int index) {
    ::assert(index >= 0);
    ::assert(index < this.size());
    return this._a[index];
  }

  void set(int index, T v) {
    ::assert(index >= 0);
    ::assert(index < this.size());
    this._a[index] = v;
  }

  void release() {
    this.construct();
  }
};

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_HEAP_PTR_H)B2HDR"},
                        {"b2style/loaded_method.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_LOADED_METHOD_H
#define B2STYLE_LIB_B2STYLE_LOADED_METHOD_H

#include <b2style/types.h>
#include <bstyle/load_method.h>

namespace b2style {

void load_method(string m) {
  ::load_method(m);
}

template <T>
T execute_loaded_method() {
  T result;
#ifdef B3STYLE
  logic "interrupt execute_loaded_method @@prefixes@temps@string result";
#else #ifdef B2STYLE
  logic "interrupt execute_loaded_method @@prefixes@temps@string b2style__result";
#else
  // TODO: Trigger an #error
#endif
#endif
  return result;
}

template <T, RT>
RT execute_loaded_method(T p) {
  RT result;
#ifdef B3STYLE
  logic "interrupt execute_loaded_method p result";
#else #ifdef B2STYLE
  logic "interrupt execute_loaded_method b2style__p b2style__result";
#else
  // TODO: Trigger an #error
#endif
#endif
  return result;
}
	
}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_LOADED_METHOD_H
)B2HDR"},
                        {"b2style/operators.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_OPERATORS_H
#define B2STYLE_LIB_B2STYLE_OPERATORS_H

#include <b2style/types.h>
#include <bstyle/int.h>
#include <bstyle/str.h>

namespace b2style {

bool and(bool i, bool j) {
  if (i) return j;
  return false;
}

bool or(bool i, bool j) {
  if (i) return true;
  if (j) return true;
  return false;
}

bool not(bool i) {
  if (i) return false;
  return true;
}

// TODO: Consider to avoid adding "_pre" for operator !.
bool not_pre(bool i) {
  return not(i);
}

bool equal(string i, string j) {
  return ::str_equal(i, j);
}

bool not_equal(string i, string j) {
  return not(equal(i, j));
}

bool equal(biguint i, biguint j) {
  return ::equal(i, j);
}

bool not_equal(biguint i, biguint j) {
  return not(equal(i, j));
}

bool equal(long i, long j) {
  return ::equal(i, j);
}

bool not_equal(long i, long j) {
  return not(equal(i, j));
}

bool equal(int i, int j) {
  return ::equal(i, j);
}

bool not_equal(int i, int j) {
  return not(equal(i, j));
}

bool equal(bool i, bool j) {
  if (i) return j;
  return not(j);
}

bool not_equal(bool i, bool j) {
  return not(equal(i, j));
}

bool greater_than(biguint i, biguint j) {
  bool result;
#ifdef B3STYLE
  logic "more result i j";
#else
  logic "more b2style__result b2style__i b2style__j";
#endif
  return result;
}

bool greater_than(long i, long j) {
  return greater_than(::to_biguint(i), ::to_biguint(j));
}

bool greater_than(int i, int j) {
  return greater_than(::to_biguint(i), ::to_biguint(j));
}

bool less_than(biguint i, biguint j) {
  bool result;
#ifdef B3STYLE
  logic "less result i j";
#else
  logic "less b2style__result b2style__i b2style__j";
#endif
  return result;
}

bool less_than(long i, long j) {
  return less_than(::to_biguint(i), ::to_biguint(j));
}

bool less_than(int i, int j) {
  return less_than(::to_biguint(i), ::to_biguint(j));
}

bool less_or_equal(biguint i, biguint j) {
  return or(less_than(i, j), equal(i, j));
}

bool less_or_equal(long i, long j) {
  return or(less_than(i, j), equal(i, j));
}

bool less_or_equal(int i, int j) {
  return or(less_than(i, j), equal(i, j));
}

bool greater_or_equal(biguint i, biguint j) {
  return or(greater_than(i, j), equal(i, j));
}

bool greater_or_equal(long i, long j) {
  return or(greater_than(i, j), equal(i, j));
}

bool greater_or_equal(int i, int j) {
  return or(greater_than(i, j), equal(i, j));
}

biguint add(biguint i, biguint j) {
#ifdef B3STYLE
  logic "add i i j";
#else
  logic "add b2style__i b2style__i b2style__j";
#endif
  return i;
}

long add(long i, long j) {
#ifdef B3STYLE
  logic "add i i j";
#else
  logic "add b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_long(i);
}

int add(int i, int j) {
#ifdef B3STYLE
  logic "add i i j";
#else
  logic "add b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_int(i);
}

byte add(byte i, byte j) {
#ifdef B3STYLE
  logic "add i i j";
#else
  logic "add b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_byte(i);
}

string add(string i, string j) {
  return ::str_concat(i, j);
}

biguint minus(biguint i, biguint j) {
#ifdef B3STYLE
  logic "subtract i i j";
#else
  logic "subtract b2style__i b2style__i b2style__j";
#endif
  return i;
}

long minus(long i, long j) {
#ifdef B3STYLE
  logic "subtract i i j";
#else
  logic "subtract b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_long(i);
}

int minus(int i, int j) {
#ifdef B3STYLE
  logic "subtract i i j";
#else
  logic "subtract b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_int(i);
}

biguint multiply(biguint i, biguint j) {
#ifdef B3STYLE
  logic "multiply i i j";
#else
  logic "multiply b2style__i b2style__i b2style__j";
#endif
  return i;
}

long multiply(long i, long j) {
#ifdef B3STYLE
  logic "multiply i i j";
#else
  logic "multiply b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_long(i);
}

int multiply(int i, int j) {
#ifdef B3STYLE
  logic "multiply i i j";
#else
  logic "multiply b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_int(i);
}

biguint divide(biguint i, biguint j) {
  biguint result;
#ifdef B3STYLE
  logic "divide result @@prefixes@temps@string i j";
#else
  logic "divide b2style__result @@prefixes@temps@string b2style__i b2style__j";
#endif
  return result;
}

long divide(long i, long j) {
  long result;
#ifdef B3STYLE
  logic "divide result @@prefixes@temps@string i j";
#else
  logic "divide b2style__result @@prefixes@temps@string b2style__i b2style__j";
#endif
  return ::fit_in_long(result);
}

int divide(int i, int j) {
  int result;
#ifdef B3STYLE
  logic "divide result @@prefixes@temps@string i j";
#else
  logic "divide b2style__result @@prefixes@temps@string b2style__i b2style__j";
#endif
  return ::fit_in_int(result);
}

biguint mod(biguint i, biguint j) {
  biguint result;
#ifdef B3STYLE
  logic "divide @@prefixes@temps@string result i j";
#else
  logic "divide @@prefixes@temps@string b2style__result b2style__i b2style__j";
#endif
  return result;
}

long mod(long i, long j) {
  long result;
#ifdef B3STYLE
  logic "divide @@prefixes@temps@string result i j";
#else
  logic "divide @@prefixes@temps@string b2style__result b2style__i b2style__j";
#endif
  return ::fit_in_long(result);
}

int mod(int i, int j) {
  int result;
#ifdef B3STYLE
  logic "divide @@prefixes@temps@string result i j";
#else
  logic "divide @@prefixes@temps@string b2style__result b2style__i b2style__j";
#endif
  return ::fit_in_int(result);
}

biguint power(biguint i, biguint j) {
#ifdef B3STYLE
  logic "power i i j";
#else
  logic "power b2style__i b2style__i b2style__j";
#endif
  return i;
}

long power(long i, long j) {
#ifdef B3STYLE
  logic "power i i j";
#else
  logic "power b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_long(i);
}

int power(int i, int j) {
#ifdef B3STYLE
  logic "power i i j";
#else
  logic "power b2style__i b2style__i b2style__j";
#endif
  return ::fit_in_int(i);
}

biguint bit_and(biguint i, biguint j) {
#ifdef B3STYLE
  logic "and i i j";
#else
  logic "and b2style__i b2style__i b2style__j";
#endif
  return i;
}

long bit_and(long i, long j) {
#ifdef B3STYLE
  logic "and i i j";
#else
  logic "and b2style__i b2style__i b2style__j";
#endif
  return i;
}

int bit_and(int i, int j) {
#ifdef B3STYLE
  logic "and i i j";
#else
  logic "and b2style__i b2style__i b2style__j";
#endif
  return i;
}

biguint bit_or(biguint i, biguint j) {
#ifdef B3STYLE
  logic "or i i j";
#else
  logic "or b2style__i b2style__i b2style__j";
#endif
  return i;
}

long bit_or(long i, long j) {
#ifdef B3STYLE
  logic "or i i j";
#else
  logic "or b2style__i b2style__i b2style__j";
#endif
  return i;
}

int bit_or(int i, int j) {
#ifdef B3STYLE
  logic "or i i j";
#else
  logic "or b2style__i b2style__i b2style__j";
#endif
  return i;
}

biguint self_inc_post(biguint& x) {
  biguint r = x;
#ifdef B3STYLE
  logic "add x x @@prefixes@constants@int_1";
#else
  logic "add b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return r;
}

long self_inc_post(long& x) {
  long r = x;
#ifdef B3STYLE
  logic "add x x @@prefixes@constants@int_1";
#else
  logic "add b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return ::fit_in_long(r);
}

int self_inc_post(int& x) {
  int r = x;
#ifdef B3STYLE
  logic "add x x @@prefixes@constants@int_1";
#else
  logic "add b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return ::fit_in_int(r);
}

biguint self_dec_post(biguint& x) {
  biguint r = x;
#ifdef B3STYLE
  logic "subtract x x @@prefixes@constants@int_1";
#else
  logic "subtract b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return r;
}

long self_dec_post(long& x) {
  long r = x;
#ifdef B3STYLE
  logic "subtract x x @@prefixes@constants@int_1";
#else
  logic "subtract b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return ::fit_in_long(r);
}

int self_dec_post(int& x) {
  int r = x;
#ifdef B3STYLE
  logic "subtract x x @@prefixes@constants@int_1";
#else
  logic "subtract b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return ::fit_in_int(r);
}

biguint self_inc_pre(biguint& x) {
#ifdef B3STYLE
  logic "add x x @@prefixes@constants@int_1";
#else
  logic "add b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return x;
}

long self_inc_pre(long& x) {
#ifdef B3STYLE
  logic "add x x @@prefixes@constants@int_1";
#else
  logic "add b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return ::fit_in_long(x);
}

int self_inc_pre(int& x) {
#ifdef B3STYLE
  logic "add x x @@prefixes@constants@int_1";
#else
  logic "add b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return ::fit_in_int(x);
}

biguint self_dec_pre(biguint& x) {
#ifdef B3STYLE
  logic "subtract x x @@prefixes@constants@int_1";
#else
  logic "subtract b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return x;
}

long self_dec_pre(long& x) {
#ifdef B3STYLE
  logic "subtract x x @@prefixes@constants@int_1";
#else
  logic "subtract b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return ::fit_in_long(x);
}

int self_dec_pre(int& x) {
#ifdef B3STYLE
  logic "subtract x x @@prefixes@constants@int_1";
#else
  logic "subtract b2style__x b2style__x @@prefixes@constants@int_1";
#endif
  return ::fit_in_int(x);
}

biguint extract(biguint i, biguint j) {
  biguint r;
#ifdef B3STYLE
  logic "extract r @@prefixes@temps@biguint i j";
#else
  logic "extract b2style__r @@prefixes@temps@biguint b2style__i b2style__j";
#endif
  return r;
}

biguint extract_remainder(biguint i, biguint j) {
  biguint r;
#ifdef B3STYLE
  logic "extract @@prefixes@temps@biguint r i j";
#else
  logic "extract @@prefixes@temps@biguint b2style__r b2style__i b2style__j";
#endif
  return r;
}

biguint left_shift(biguint i, biguint j) {
  biguint r;
#ifdef B3STYLE
  logic "left_shift r i j";
#else
  logic "left_shift b2style__r b2style__i b2style__j";
#endif
  return r;
}

biguint right_shift(biguint i, biguint j) {
  biguint r;
#ifdef B3STYLE
  logic "right_shift r i j";
#else
  logic "right_shift b2style__r b2style__i b2style__j";
#endif
  return r;
}

biguint left_shift(biguint i, int j) {
  return left_shift(i, ::to_biguint(j));
}

biguint right_shift(biguint i, int j) {
  return right_shift(i, ::to_biguint(j));
}

biguint left_shift(biguint i, long j) {
  return left_shift(i, ::to_biguint(j));
}

biguint right_shift(biguint i, long j) {
  return right_shift(i, ::to_biguint(j));
}

int left_shift(int i, int j) {
  return ::to_int(
    left_shift(::to_biguint(i),
               ::to_biguint(j))
  );
}

int right_shift(int i, int j) {
  return ::to_int(
    right_shift(::to_biguint(i),
                ::to_biguint(j))
  );
}

long left_shift(long i, long j) {
  return ::to_long(
    left_shift(::to_biguint(i),
               ::to_biguint(j))
  );
}

long right_shift(long i, long j) {
  return ::to_long(
    right_shift(::to_biguint(i),
                ::to_biguint(j))
  );
}

void self_and(bool& i, bool j) {
  i = and(i, j);
}

void self_or(bool& i, bool j) {
  i = or(i, j);
}

void self_add(biguint& i, biguint j) {
  i = add(i, j);
}

void self_add(long& i, long j) {
  i = add(i, j);
}

void self_add(int& i, int j) {
  i = add(i, j);
}

void self_add(byte& i, byte j) {
  i = add(i, j);
}

void self_minus(biguint& i, biguint j) {
  i = minus(i, j);
}

void self_minus(long& i, long j) {
  i = minus(i, j);
}

void self_minus(int& i, int j) {
  i = minus(i, j);
}

void self_multiply(biguint& i, biguint j) {
  i = multiply(i, j);
}

void self_multiply(long& i, long j) {
  i = multiply(i, j);
}

void self_multiply(int& i, int j) {
  i = multiply(i, j);
}

void self_divide(biguint& i, biguint j) {
  i = divide(i, j);
}

void self_divide(long& i, long j) {
  i = divide(i, j);
}

void self_divide(int& i, int j) {
  i = divide(i, j);
}

void self_mod(biguint& i, biguint j) {
  i = mod(i, j);
}

void self_mod(long& i, long j) {
  i = mod(i, j);
}

void self_mod(int& i, int j) {
  i = mod(i, j);
}

void self_power(biguint& i, biguint j) {
  i = power(i, j);
}

void self_power(long& i, long j) {
  i = power(i, j);
}

void self_power(int& i, int j) {
  i = power(i, j);
}

void self_bit_and(biguint& i, biguint j) {
  i = bit_and(i, j);
}

void self_bit_and(long& i, long j) {
  i = bit_and(i, j);
}

void self_bit_and(int& i, int j) {
  i = bit_and(i, j);
}

void self_bit_or(biguint& i, biguint j) {
  i = bit_or(i, j);
}

void self_bit_or(long& i, long j) {
  i = bit_or(i, j);
}

void self_bit_or(int& i, int j) {
  i = bit_or(i, j);
}

void self_left_shift(biguint& i, biguint j) {
  i = left_shift(i, j);
}

void self_right_shift(biguint& i, biguint j) {
  i = right_shift(i, j);
}

void self_left_shift(long& i, long j) {
  i = left_shift(i, j);
}

void self_right_shift(long& i, long j) {
  i = right_shift(i, j);
}

void self_left_shift(int& i, int j) {
  i = left_shift(i, j);
}

void self_right_shift(int& i, int j) {
  i = right_shift(i, j);
}

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_OPERATORS_H
)B2HDR"},
                        {"b2style/raw_heap_ptr.h", R"B2HDR(

/* TODO: Implementation
#ifndef B2STYLE_LIB_B2STYLE_RAW_HEAP_PTR
#define B2STYLE_LIB_B2STYLE_RAW_HEAP_PTR

#include <b2style.h>
#include <assert.h>

namespace b2style {

class raw_heap_ptr {
  type_ptr p;

#ifdef B3STYLE
  bool is_null() {
    bool r;
    logic "empty r this.p";
    return r;
  }
#else
  bool is_null() {
    bool r;
    logic "empty b2style__r this.p";
    return r;
  }
#endif

  template <T>
  T get() {
    type_ptr v = this.p;
    static_cast(v, T);
    return v;
  }

  void set_null() {
    type_ptr x;
    this.p = x;
  }

  // TODO: Find a good way to automatically dealloc heap resources.
  template <T>
  void destruct() {
    ::assert(!this.is_null());
    T x = this.get<T>();
    dealloc(x);
    type_ptr x;
    this.p = x;
  }

  template <T>
  void set(T t) {
    static_cast(t, type_ptr);
    this.p = t;
  }
};

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_RAW_HEAP_PTR
*/
)B2HDR"},
                        {"b2style/ref.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_REF_H
#define B2STYLE_LIB_B2STYLE_REF_H

#include <b2style.h>
#include <b2style/heap_ptr.h>
#include <assert.h>

namespace b2style {

template <T>
class ref {
  heap_ptr<T> _a;

  void destruct() {
    this._a.destruct();
  }

  bool empty() {
    return this._a.empty();
  }

  void set(T v) {
    if (this.empty()) this._a.alloc(1);
    this._a.set(0, v);
  }

  void alloc() {
    T v;
    this.set(v);
  }

  T get() {
    ::assert(!this.empty());
    return this._a.get(0);
  }

  T release() {
    T r = this.get();
	this.destruct();
    return r;
  }

  void construct() {}

  void construct(T v) {
    this.construct();
    this.set(v);
  }

  void construct(ref<T>& other) {
    this.construct(other.release());
  }
};

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_REF_H
)B2HDR"},
                        {"b2style/stdio.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_STDIO_H
#define B2STYLE_LIB_B2STYLE_STDIO_H

#include <b2style/to_str.h>
#include <b2style/types.h>
#include <stdio.h>

namespace b2style {

void std_out(string i) {
  ::std_out(i);
}

void std_err(string i) {
  ::std_err(i);
}

template <T>
void std_out(T i) {
  std_out(to_str(i));
}

template <T>
void std_err(T i) {
  std_err(to_str(i));
}

// TODO: Implement template inference.
void std_out(bool i) {
  std_out<bool>(i);
}

void std_err(bool i) {
  std_err<bool>(i);
}

void std_out(int i) {
  std_out<int>(i);
}

void std_err(int i) {
  std_err<int>(i);
}

void std_out(biguint i) {
  std_out<biguint>(i);
}

void std_err(biguint i) {
  std_err<biguint>(i);
}

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_STDIO_H
)B2HDR"},
                        {"b2style/str.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_STR_H
#define B2STYLE_LIB_B2STYLE_STR_H

#include <b2style/types.h>
#include <bstyle/str.h>
#include <assert.h>

namespace b2style {

string str_mid(string s, int i, int l) {
  ::assert(__STATEMENT__ + "@" + __FILE__, (i >= 0) && (l >= 0));
  string r;
#ifdef B3STYLE
  logic "cut_len r s i l";
#else #ifdef B2STYLE
  logic "cut_len b2style__r b2style__s b2style__i b2style__l";
#else
  // TODO: Trigger an #error.
#endif
#endif
  return r;
}

bool str_ends_with(string i, string j) {
  int il = ::str_len(i);
  int jl = ::str_len(j);
  if (il < jl) {
    return false;
  }
  if (il == jl) {
    return ::str_equal(i, j);
  }
  string si = str_mid(i, il - jl, jl);
  return ::str_equal(si, j);
}

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_STR_H
)B2HDR"},
                        {"b2style/to_str.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_TO_STR_H
#define B2STYLE_LIB_B2STYLE_TO_STR_H

#include <b2style/loaded_method.h>
#include <b2style/operators.h>
#include <limits.h>

namespace b2style {

string to_str(string i) {
  return i;
}

string to_str(byte i) {
  return ::to_str(i);
}

string to_str(bool i) {
  if (i) return "True";
  return "False";
}

string legacy_biguint_to_str(biguint i) {
  if (i == 0L) {
    return "0";
  }
  string s;
  while (i > 0L) {
    int b = ::to_int(mod(i, 10L));
    i /= 10L;
    b += 48;
    s = ::str_concat(::to_str(::to_byte(b)), s);
  }
  return s;
}

string biguint_to_str(biguint i) {
  load_method("big_uint_to_str");
  return execute_loaded_method<biguint, string>(i);
}

string to_str(biguint i) {
  return biguint_to_str(i);
}

template <T>
string biguint_to_str_forward(T i, T MAX) {
  if (i <= MAX) {
    return biguint_to_str(::to_biguint(i));
  }
  i -= MAX;
  i = MAX - i;
  T _2 = 2;
  i += _2;
  return ::str_concat("-", biguint_to_str(::to_biguint(i)));
}

string int_to_str(int i) {
  return biguint_to_str_forward<int>(i, ::INT_MAX);
}

string ufloat_to_str(ufloat i) {
  load_method("big_udec_to_str");
  return execute_loaded_method<ufloat, string>(i);
}

string long_to_str(long i) {
  return biguint_to_str_forward<long>(i, ::LONG_MAX);
}

string to_str(int i) {
  return int_to_str(i);
}

string to_str(ufloat i) {
  return ufloat_to_str(i);
}

string to_str(long i) {
  return long_to_str(i);
}

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_TO_STR_H
)B2HDR"},
                        {"b2style/types.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_TYPES_H
#define B2STYLE_LIB_B2STYLE_TYPES_H

#include <bstyle/types.h>

// TODO: Search types in parent scopes.
namespace b2style {

typedef ::string string;
typedef ::void void;
typedef ::bool bool;
typedef ::biguint biguint;
typedef ::long long;
typedef ::int int;
typedef ::byte byte;
typedef ::ufloat ufloat;

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_TYPES_H
)B2HDR"},
                        {"b2style/ufloat.h", R"B2HDR(

#ifndef B2STYLE_LIB_B2STYLE_UFLOAT_H
#define B2STYLE_LIB_B2STYLE_UFLOAT_H

#include <b2style/loaded_method.h>
#include <b2style/stdio.h>
#include <b2style/str.h>
#include <b2style/types.h>

namespace b2style {

bool equal(ufloat i, ufloat j) {
  bool result;
#ifdef B3STYLE
  logic "float_equal result i j";
#else
  logic "float_equal b2style__result b2style__i b2style__j";
#endif
  return result;
}

bool not_equal(ufloat i, ufloat j) {
  bool result;
#ifdef B3STYLE
  logic "float_equal result i j";
#else
  logic "float_equal b2style__result b2style__i b2style__j";
#endif
  return not(result);
}

bool greater_than(ufloat i, ufloat j) {
  bool result;
#ifdef B3STYLE
  logic "float_more result i j";
#else
  logic "float_more b2style__result b2style__i b2style__j";
#endif
  return result;
}

bool less_than(ufloat i, ufloat j) {
  bool result;
#ifdef B3STYLE
  logic "float_less result i j";
#else
  logic "float_less b2style__result b2style__i b2style__j";
#endif
  return result;
}

ufloat add(ufloat i, ufloat j) {
#ifdef B3STYLE
  logic "float_add i i j";
#else
  logic "float_add b2style__i b2style__i b2style__j";
#endif
  return i;
}

ufloat minus(ufloat i, ufloat j) {
#ifdef B3STYLE
  logic "float_subtract i i j";
#else
  logic "float_subtract b2style__i b2style__i b2style__j";
#endif
  return i;
}

ufloat multiply(ufloat i, ufloat j) {
#ifdef B3STYLE
  logic "float_multiply i i j";
#else
  logic "float_multiply b2style__i b2style__i b2style__j";
#endif
  return i;
}

ufloat divide(ufloat i, ufloat j) {
  ufloat result;
#ifdef B3STYLE
  logic "float_divide result i j";
#else
  logic "float_divide b2style__result b2style__i b2style__j";
#endif
  return result;
}

ufloat power(ufloat i, ufloat j) {
#ifdef B3STYLE
  logic "float_power i i j";
#else
  logic "float_power b2style__i b2style__i b2style__j";
#endif
  return i;
}

bool greater_or_equal(ufloat i, ufloat j) {
  return or(greater_than(i, j), equal(i, j));
}

bool less_or_equal(ufloat i, ufloat j) {
  return or(less_than(i, j), equal(i, j));
}

void std_out(ufloat i) {
  std_out(ufloat_to_str(i));
}

void std_err(ufloat i) {
  std_err(ufloat_to_str(i));
}

ufloat self_inc_post(ufloat& x) {
  ufloat r = x;
	x = add(x, 1.0);
  return r;
}

ufloat self_dec_post(ufloat& x) {
  ufloat r = x;
	x = minus(x, 1.0);
  return r;
}

ufloat self_inc_pre(ufloat& x) {
	x = add(x, 1.0);
  return x;
}

ufloat self_dec_pre(ufloat& x) {
	x = minus(x, 1.0);
  return x;
}

void self_add(ufloat& i, ufloat j) {
  i = add(i, j);
}

void self_minus(ufloat& i, ufloat j) {
  i = minus(i, j);
}

void self_multiply(ufloat& i, ufloat j) {
  i = multiply(i, j);
}

void self_divide(ufloat& i, ufloat j) {
  i = divide(i, j);
}

void self_power(ufloat& i, ufloat j) {
  i = power(i, j);
}

namespace ufloat {

typedef ::string string;
typedef ::void void;
typedef ::bool bool;
typedef ::biguint biguint;
typedef ::long long;
typedef ::int int;
typedef ::byte byte;
typedef ::ufloat ufloat;

ufloat from(biguint i) {
  ::b2style::load_method("big_uint_to_big_udec");
  return ::b2style::execute_loaded_method<biguint, ufloat>(i);
}

ufloat from(int i) {
  return from(::to_biguint(i));
}

ufloat from(long i) {
  return from(::to_biguint(i));
}

ufloat fraction(biguint n, biguint d) {
  ufloat result = from(n);
  ufloat ud = from(d);
  return ::b2style::divide(result, ud);
}

ufloat fraction(biguint n, int d) {
  return fraction(n, ::to_biguint(d));
}

ufloat fraction(biguint n, long d) {
  return fraction(n, ::to_biguint(d));
}

ufloat fraction(int n, biguint d) {
  return fraction(::to_biguint(n), d);
}

ufloat fraction(long n, biguint d) {
  return fraction(::to_biguint(n), d);
}

ufloat fraction(int n, int d) {
  return fraction(::to_biguint(n), ::to_biguint(d));
}

ufloat fraction(long n, long d) {
  return fraction(::to_biguint(n), ::to_biguint(d));
}

}  // namespace ufloat

}  // namespace b2style
#endif  // B2STYLE_LIB_B2STYLE_UFLOAT_H
)B2HDR"},
                        {"std/types.h", R"B2HDR(

#ifndef B2STYLE_LIB_STD_TYPES_H
#define B2STYLE_LIB_STD_TYPES_H

#include <bstyle/types.h>

namespace std {

typedef ::string string;
typedef ::void void;
typedef ::bool bool;
typedef ::biguint biguint;
typedef ::long long;
typedef ::int int;
typedef ::byte byte;
typedef ::ufloat ufloat;

}  // namespace std

#endif  // B2STYLE_LIB_STD_TYPES_H)B2HDR"},
                        {"std/vector", R"B2HDR(

#ifndef B2STYLE_LIB_STD_VECTOR
#define B2STYLE_LIB_STD_VECTOR

#include <b2style.h>
#include <assert.h>
#include <b2style/heap_ptr.h>
#include <std/types.h>

namespace std {

template <T>
class vector {
  ::b2style::heap_ptr<T> _a;
  int _s;

  int size() {
    return this._s;
  }

  int capacity() {
    return this._a.size();
  }

  T get(int index) {
    ::assert(index >= 0, "get");
    ::assert(index < this.size(), "get");
    return this._a.get(index);
  }

  void set(int index, T v) {
    ::assert(index >= 0, "set");
    ::assert(index < this.size(), "set");
    this._a.set(index, v);
  }

  void destruct() {
    this._a.destruct();
  }

  void reserve(int size) {
    ::b2style::heap_ptr<T> x(size);
    for (int i = 0; i < this.size(); i++) {
      x.set(i, this.get(i));
    }
	this.destruct();
    this._a = x;
	x.release();
  }

  void construct(int size) {
    this.reserve(size);
  }

  void construct() {
    this.construct(4);
  }

  void push_back(T v) {
    if (this.size() == this.capacity()) {
      this.reserve(this.capacity() << 1);
    }
    this._s++;
    this.set(this.size() - 1, v);
  }

  void pop_back() {
    ::assert(this.size() > 0, "pop_back");
    this._s--;
  }

  void clear() {
    this._s = 0;
  }
};

}  // namespace std

#endif  // B2STYLE_LIB_STD_VECTOR)B2HDR"},
                        {"testing.h", R"B2HDR(

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
)B2HDR"},
                        {"testing/assert.h", R"B2HDR(

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
)B2HDR"},
                        {"testing/types.h", R"B2HDR(

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

#endif  // B2STYLE_LIB_TESTING_TYPES_H)B2HDR"}
                    };
                    return m;
                }

                static bool find_header(const std::string& path, std::string& content)
                {
                    const auto& m = files();
                    auto it = m.find(path);
                    if (it != m.end())
                    {
                        content = it->second;
                        return true;
                    }
                    return false;
                }
            };
        }
    }
}
