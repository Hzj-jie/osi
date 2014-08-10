
#pragma once
#include <boost/predef.h>
#include <atomic>
#include <memory>
#include <utility>
#include "../app_info/assert.hpp"

#if BOOST_COMP_MSVC
#include "lock.hpp"
#include <mutex>
#endif

template <typename T>
struct atomic_shared_ptr final
{
private:
    std::shared_ptr<T> v;
#if BOOST_COMP_MSVC
    std::mutex mtx;
#endif

    bool has_value() const
    {
#if BOOST_COMP_MSVC
        scope_lock(mtx);
        return (bool)(v);
#else
        return (bool)(atomic_load_explicit(&v, std::memory_order_consume));
#endif
    }

public:
    atomic_shared_ptr() { }

    static bool is_lockfree()
    {
#if BOOST_COMP_MSVC
        return false;
#else
        return std::atomic_is_lock_free(std::shared_ptr<T>());
#endif
    }

    template <typename U>
    void reset(U* p)
    {
#if BOOST_COMP_MSVC
        scope_lock(mtx);
        v.reset(p);
#else
        std::atomic_store_explicit(&v, std::make_shared<T>(p), std::memory_order_consume);
#endif
    }

    template <typename U>
    atomic_shared_ptr& operator=(const std::shared_ptr<U>& x)
    {
#if BOOST_COMP_MSVC
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
#if BOOST_COMP_MSVC
        scope_lock(mtx);
        v = x.v;
#else
        std::atomic_store_explicit(&v, x.v, std::memory_order_consume);
#endif
        return *this;
    }

    template <typename U>
    atomic_shared_ptr(U&& i)
    {
        operator=(i);
    }

public:
    operator std::shared_ptr<T>() const
    {
#if BOOST_COMP_MSVC
        scope_lock(mtx);
        return v;
#else
        return std::atomic_load_explicit(v, std::memory_order_consume);
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
#if BOOST_COMP_MSVC
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
#if BOOST_COMP_MSVC
        scope_lock(mtx);
        scope_lock(i.mtx);
        v.swap(i.v);
#else
        i.v = std::make_shared<T>(
                  std::atomic_exchange_explicit(&v,
                      std::atomic_load_explicit(i.v,
                                                std::memory_order_consume)),
                      std::memory_order_consume);
#endif
    }

    template <typename U>
    void compare_exchange_weak(std::shared_ptr<T>* expected,
                               std::shared_ptr<T> desired)
    {
        return std::atomic_compare_exchange_weak_explicit(
                    &v,
                    expected,
                    desired,
                    std::memory_order_consume);
    }

    template <typename U>
    void compare_exchange_strong(std::shared_ptr<T>* expected,
                                 std::shared_ptr<T> desired)
    {
        return std::atomic_compare_exchange_strong_explicit(
                    &v,
                    expected,
                    desired,
                    std::memory_order_consume);
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

