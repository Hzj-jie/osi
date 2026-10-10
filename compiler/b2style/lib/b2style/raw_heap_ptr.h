
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
