
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
