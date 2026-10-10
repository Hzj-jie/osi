
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