
#ifndef B2STYLE_LIB_B2STYLE_DELEGATES_H
#define B2STYLE_LIB_B2STYLE_DELEGATES_H

#include <b2style/types.h>

namespace b2style {

template <RT>
delegate RT function();

template <T, RT>
delegate RT function(T);

template <T, T2, RT>
delegate RT function(T, T2);

template <T, T2, T3, RT>
delegate RT function(T, T2, T3);

template <T, T2, T3, T4, RT>
delegate RT function(T, T2, T3, T4);

template <T, T2, T3, T4, T5, RT>
delegate RT function(T, T2, T3, T4, T5);

template <T, T2, T3, T4, T5, T6, RT>
delegate RT function(T, T2, T3, T4, T5, T6);

template <T, T2, T3, T4, T5, T6, T7, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7);

template <T, T2, T3, T4, T5, T6, T7, T8, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15);

template <T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, RT>
delegate RT function(T, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16);

}  // namespace b2style

#endif  // B2STYLE_LIB_B2STYLE_DELEGATES_H