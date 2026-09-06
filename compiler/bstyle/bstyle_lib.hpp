#pragma once
#include <string>
#include <unordered_map>

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class bstyle_lib
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

reference &

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

root-type base-root-type

raw-name-with-dot raw-name dot
name [raw-name-with-dot]* raw-name
type-name raw-name
sentence-with-semi-colon base-sentence-with-semi-colon
for-increase base-for-increase
value-without-bracket base-value-without-bracket
raw-variable-name name
variable-name [heap-name, raw-variable-name]
)";
                    return rule;
                }

                static const std::unordered_map<std::string, std::string>& files()
                {
                    static const std::unordered_map<std::string, std::string> m = {
                        {"bstyle.h", R"(
#ifndef BSTYLE_LIB_BSTYLE_H
#define BSTYLE_LIB_BSTYLE_H

#include <bstyle/types.h>
#include <bstyle/const.h>

#endif  // BSTYLE_LIB_BSTYLE_H
)"},
                        {"bstyle/types.h", R"(
#ifndef BSTYLE_LIB_BSTYLE_TYPES_H
#define BSTYLE_LIB_BSTYLE_TYPES_H

typedef Integer int;
logic "type long 8";
typedef Boolean bool;
logic "type byte 1";
typedef BigUnsignedInteger biguint;
typedef BigUnsignedFloat ufloat;
typedef String string;
// This constant needs to match the implementation in logic/scope.type_t.zero_type.
typedef type0 void;

#endif  // BSTYLE_LIB_BSTYLE_TYPES_H
)"},
                        {"bstyle/const.h", R"(
#ifndef BSTYLE_LIB_BSTYLE_CONSTANTS_H
#define BSTYLE_LIB_BSTYLE_CONSTANTS_H

logic "define @@prefixes@constants@int_0 Integer";
logic "copy_const @@prefixes@constants@int_0 i0";

logic "define @@prefixes@constants@int_1 Integer";
logic "copy_const @@prefixes@constants@int_1 i1";

logic "define @@prefixes@constants@size_of_int Integer";
logic "copy_const @@prefixes@constants@size_of_int i4";

logic "define @@prefixes@constants@size_of_long Integer";
logic "copy_const @@prefixes@constants@size_of_long i8";

logic "define @@prefixes@constants@size_of_bool Integer";
logic "copy_const @@prefixes@constants@size_of_bool i1";

logic "define @@prefixes@constants@size_of_byte Integer";
logic "copy_const @@prefixes@constants@size_of_byte i1";

logic "define @@prefixes@constants@size_of_float Integer";
logic "copy_const @@prefixes@constants@size_of_float i16";

logic "define @@prefixes@constants@eof Integer";
logic "copy_const @@prefixes@constants@eof i-1";

logic "define @@prefixes@constants@ptr_offset type_ptr";
logic "copy_const @@prefixes@constants@ptr_offset l4294967296";

logic "define @@prefixes@temps@biguint BigUnsignedInteger";
logic "define @@prefixes@temps@string String";

#endif  // BSTYLE_LIB_BSTYLE_CONSTANTS_H
)"},
                        {"bstyle/int.h", R"(
#ifndef BSTYLE_LIB_BSTYLE_INT_H
#define BSTYLE_LIB_BSTYLE_INT_H

#include <bstyle/const.h>
#include <bstyle/types.h>

biguint to_biguint(int i) {
  return i;
}

biguint to_biguint(long i) {
  return i;
}

long to_long(int i) {
  return i;
}

long fit_in_long(long& x) {
  logic "cut_len x x @@prefixes@constants@int_0 @@prefixes@constants@size_of_long";
  return x;
}

int fit_in_int(int& x) {
  logic "cut_len x x @@prefixes@constants@int_0 @@prefixes@constants@size_of_int";
  return x;
}

byte fit_in_byte(byte& x) {
  logic "cut_len x x @@prefixes@constants@int_0 @@prefixes@constants@size_of_byte";
  return x;
}

long to_long(biguint x) {
  long y;
  logic "cut_len y x @@prefixes@constants@int_0 @@prefixes@constants@size_of_long";
  return y;
}

int to_int(biguint x) {
  int y;
  logic "cut_len y x @@prefixes@constants@int_0 @@prefixes@constants@size_of_int";
  return y;
}

byte to_byte(int x) {
  byte y;
  logic "cut_len y x @@prefixes@constants@int_0 @@prefixes@constants@size_of_byte";
  return y;
}

bool equal(biguint i, biguint j) {
  bool result;
  logic "equal result i j";
  return result;
}

bool equal(long i, long j) {
  return equal(to_biguint(i), to_biguint(j));
}

bool equal(int i, int j) {
  return equal(to_biguint(i), to_biguint(j));
}

#endif  // BSTYLE_LIB_BSTYLE_INT_H
)"},
                        {"bstyle/str.h", R"(
#ifndef BSTYLE_LIB_BSTYLE_STR_H
#define BSTYLE_LIB_BSTYLE_STR_H

#include <bstyle/int.h>
#include <bstyle/types.h>

string str_concat(string i, string j) {
  logic "append i j";
  return i;
}

string str_concat(string i, string j, string k) {
  return str_concat(str_concat(i, j), k);
}

string str_concat(string i, byte j) {
  logic "append i j";
  return i;
}

string to_str(byte i) {
  string s;
  return str_concat(s, i);
}

int str_len(string s) {
  int r;
  logic "sizeof r s";
  return r;
}

bool str_empty(string s) {
  // Note, empty means the s == null rather than s.length == 0.
  return equal(str_len(s), 0);
}

// TODO: Use a better way to compare strings, treating them as big_uints is not accurate or efficient.
bool str_equal(string i, string j) {
  bool result;
  logic "equal result i j";
  return result;
}

#endif  // BSTYLE_LIB_BSTYLE_STR_H
)"},
                        {"bstyle/time.h", R"(
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
)"},
                        {"bstyle/load_method.h", R"(
#ifndef BSTYLE_LIB_BSTYLE_LOADED_METHOD_H
#define BSTYLE_LIB_BSTYLE_LOADED_METHOD_H

#include <bstyle/types.h>
#include <bstyle/str.h>

void load_method(string m) {
  m = str_concat(
		  "osi.service.interpreter.primitive.loaded_methods, osi.service.interpreter:",
		  m);
  logic "interrupt load_method m @@prefixes@temps@string";
}

#endif  // BSTYLE_LIB_BSTYLE_LOADED_METHOD_H
)"},
                        {"stdio.h", R"(
#ifndef BSTYLE_LIB_STDIO_H
#define BSTYLE_LIB_STDIO_H

#include <bstyle/const.h>
#include <bstyle/types.h>

int getchar() {
  int r;
  logic "interrupt getchar @@prefixes@temps@string r";
  return r;
}

int putchar(int i) {
  logic "interrupt putchar i @@prefixes@temps@string";
  return i;
}

int eof() {
  int r;
  logic "copy r @@prefixes@constants@eof";
  return r;
}

void std_out(string i) {
  logic "interrupt stdout i @@prefixes@temps@string";
}

void std_err(string i) {
  logic "interrupt stderr i @@prefixes@temps@string";
}

#endif  // BSTYLE_LIB_STDIO_H
)"},
                        {"limits.h", R"(
#ifndef BSTYLE_LIB_LIMITS_H
#define BSTYLE_LIB_LIMITS_H

#include <bstyle/int.h>
#include <bstyle/types.h>

int INT_MIN = -2147483648;
int INT_MAX = 2147483647;
// TODO: Support negative long.
// long LONG_MIN = -9223372036854775808L;
long LONG_MAX = to_long(9223372036854775807L);

#endif  // BSTYLE_LIB_LIMITS_H
)"},
                        {"cstdio", R"(
#ifndef BSTYLE_LIB_CSTDIO
#define BSTYLE_LIB_CSTDIO

#include <stdio.h>

#endif  // BSTYLE_LIB_CSTDIO
)"},
                        {"climits", R"(
#ifndef BSTYLE_LIB_CLIMIT
#define BSTYLE_LIB_CLIMIT

#include <limits.h>

#endif  // BSTYLE_LIB_CLIMIT
)"}
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
