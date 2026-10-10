
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
