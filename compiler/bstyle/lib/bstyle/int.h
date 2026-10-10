
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