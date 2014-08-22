
#pragma once
#include <mutex>
#include "../utils/macro.hpp"

#ifdef scope_lock
    scope_lock redefined
#endif
#define scope_lock(x) \
            std::lock_guard<decltype(x)> random_variable_name(__SCOPE_LOCK_UNIQUE_LOCK_INSTANCE)(x);

#ifdef __static_scope_lock_mutex_name
    __static_scope_lock_mutex_name redefined
#endif
#define __static_scope_lock_mutex_name() random_variable_name(__STATIC_SCOPE_LOCK_MUTEX_INSTANCE)

#ifdef static_scope_lock
    static_scope_lock redefined
#endif
#define static_scope_lock() \
            static std::mutex __static_scope_lock_mutex_name(); \
            scope_lock(__static_scope_lock_mutex_name()); \

#ifdef static_scope_recursive_lock
    static_scope_recursive_lock redefined
#endif
#define static_scope_recursive_lock() \
            static std::recursive_mutex __static_scope_lock_mutex_name(); \
            scope_lock(__static_scope_lock_mutex_name()); \

