
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