
#pragma once
#include <mutex>
#include "../utils/macro.hpp"

#ifdef scope_lock
    scope_lock redefined
#endif
#define scope_lock(x) \
            std::lock_guard<std::mutex> random_variable_name(__SCOPE_LOCK_UNIQUE_LOCK_INSTANCE)(x);

#ifdef static_scope_lock
    static_scope_lock redefined
#endif
#define static_scope_lock() \
            static std::mutex __STATIC_SCOPE_LOCK_MUTEX_INSTANCE; \
            scope_lock(__STATIC_SCOPE_LOCK_MUTEX_INSTANCE);
