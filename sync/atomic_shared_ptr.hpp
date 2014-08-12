
#pragma once
#include <boost/predef.h>
#include <atomic>
#include <memory>
#include <utility>
#include "../app_info/assert.hpp"
#include "../sync/spin_wait.hpp"
#include <utility>

#define ATOMIC_SHARED_PTR_USE_LOCK (!BOOST_COMP_CLANG)
#if ATOMIC_SHARED_PTR_USE_LOCK
#include "lock.hpp"
#include <mutex>
#endif

template <typename T>
struct atomic_shared_ptr final
{
private:
    std::shared_ptr<T> v;
#if ATOMIC_SHARED_PTR_USE_LOCK
    mutable std::mutex mtx;
#endif

    bool has_value() const
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        scope_lock(mtx);
        return (bool)(v);
#else
        return (bool)(std::atomic_load_explicit(&v, std::memory_order_consume));
#endif
    }

public:
    atomic_shared_ptr() { }

    static bool is_lock_free()
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        return false;
#else
        return std::atomic_is_lock_free(std::shared_ptr<T>());
#endif
    }

    template <typename U>
    void reset(U* p)
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        scope_lock(mtx);
        v.reset(p);
#else
        std::atomic_store_explicit(&v, std::make_shared<T>(p), std::memory_order_consume);
#endif
    }

    template <typename U>
    atomic_shared_ptr& operator=(const std::shared_ptr<U>& x)
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        scope_lock(mtx);
        v = x;
#else
        std::atomic_store_explicit(&v, x, std::memory_order_consume);
#endif
        return *this;
    }

    template <typename U>
    atomic_shared_ptr& operator=(const atomic_shared_ptr<U>& x)
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        scope_lock(mtx);
        v = x.v;
#else
        std::atomic_store_explicit(&v, x.v, std::memory_order_consume);
#endif
        return *this;
    }

    atomic_shared_ptr& operator=(const atomic_shared_ptr<T>& x)
    {
        return operator=<T>(x);
    }

    template <typename U>
    atomic_shared_ptr(U&& i)
    {
        operator=(i);
    }

public:
    operator std::shared_ptr<T>() const
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        scope_lock(mtx);
        return v;
#else
        return std::atomic_load_explicit(&v, std::memory_order_consume);
#endif
    }

    T* get() const
    {
        return (operator std::shared_ptr<T>)().get();
    }

    T& load() const
    {
        return *get();
    }

    T* operator->() const
    {
        return get();
    }

    T& operator*() const
    {
        return load();
    }

    template <typename U>
    void swap(std::shared_ptr<U>& i)
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        scope_lock(mtx);
        v.swap(i);
#else
        i = std::make_shared<T>(
                std::atomic_exchange_explicit(&v,
                                              i,
                                              std::memory_order_consume));
#endif
    }

    template <typename U>
    void swap(atomic_shared_ptr<U>& i)
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        scope_lock(mtx);
        scope_lock2(i.mtx);
        v.swap(i.v);
#else
        i.v = std::make_shared<T>(
                  std::atomic_exchange_explicit(&v,
                      std::atomic_load_explicit(&i.v,
                                                std::memory_order_consume)),
                      std::memory_order_consume);
#endif
    }

    template <typename U>
    bool compare_exchange_weak(std::shared_ptr<U>* expected,
                               std::shared_ptr<U> desired)
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        if(expected == nullptr) return false;
        else
        {
            scope_lock(mtx);
            if(v.get() == expected->get())
            {
                v = desired;
                return true;
            }
            else
            {
                *expected = v;
                return false;
            }
        }
#else
        return std::atomic_compare_exchange_weak_explicit(
                    &v,
                    expected,
                    desired,
                    std::memory_order_consume);
#endif
    }

    template <typename U>
    bool compare_exchange_strong(std::shared_ptr<U>* expected,
                                 std::shared_ptr<U> desired)
    {
#if ATOMIC_SHARED_PTR_USE_LOCK
        std_this_thread_wait_until(compare_exchange_weak(expected, desired));
        return true;
#else
        return std::atomic_compare_exchange_strong_explicit(
                    &v,
                    expected,
                    desired,
                    std::memory_order_consume);
#endif
    }

    long int used_count() const
    {
        return v.used_count();
    }

    bool unique() const
    {
        return v.unique();
    }
};

template <typename T, typename... Args>
atomic_shared_ptr<T> make_atomic_shared(Args&&... args)
{
    return atomic_shared_ptr<T>(std::make_shared<T>(std::forward<Args>(args)...));
}

