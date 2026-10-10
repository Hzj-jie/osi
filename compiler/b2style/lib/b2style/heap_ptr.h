
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

#endif  // B2STYLE_LIB_B2STYLE_HEAP_PTR_H