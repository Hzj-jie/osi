
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
