
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