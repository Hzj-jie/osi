
#pragma once
#include <mutex>

#ifdef scope_lock
    scope_lock redefined
#endif
#define scope_lock(x) \
            std::unique_lock<std::mutex> __SCOPE_LOCK_UNIQUE_LOCK_INSTANCE(x);
#define scope_lock2(x) \
            std::unique_lock<std::mutex> __SCOPE_LOCK_UNIQUE_LOCK_INSTANCE2(x);

#ifdef static_scope_lock
    static_scope_lock redefined
#endif
#define static_scope_lock() \
            static std::mutex __STATIC_SCOPE_LOCK_MUTEX_INSTANCE; \
            scope_lock(__STATIC_SCOPE_LOCK_MUTEX_INSTANCE);
