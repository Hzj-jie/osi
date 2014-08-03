
#pragma once
#include <atomic>
#include <memory>
#include <utility>
#include "../app_info/assert.hpp"

template <typename T>
struct shared_atomic
{
private:
    std::shared_ptr<std::atomic<T> > v;
public:
    shared_atomic() { }

    void set(T i)
    {
        v = std::make_shared<std::atomic<T> >(i);
    }

    void set(const std::atomic<T>& i)
    {
        set(i.load());
    }

    void set(const std::shared_ptr<std::atomic<T> >& i)
    {
        v = i;
    }

    void set(const shared_atomic<T>& i)
    {
        v = i.raw();
    }

    shared_atomic(T i)
    {
        set(i);
    }

    shared_atomic(const std::atomic<T>& i)
    {
        set(i);
    }

    shared_atomic(const std::shared_ptr<std::atomic<T> >& i)
    {
        set(i);
    }

    shared_atomic(const shared_atomic<T>& i)
    {
        set(i);
    }

    shared_atomic<T>& operator=(T i)
    {
        set(i);
        return *this;
    }

    shared_atomic<T>& operator=(const std::atomic<T>& i)
    {
        set(i);
        return *this;
    }

    shared_atomic<T>& operator=(const std::shared_ptr<std::atomic<T> >& i)
    {
        set(i);
        return *this;
    }

    shared_atomic<T>& operator=(const shared_atomic<T>& i)
    {
        set(i);
        return *this;
    }

    std::atomic<T>* atomic() const
    {
        return v.get();
    }

private:
    bool has_value() const
    {
        return atomic() != nullptr;
    }

public:
    const std::shared_ptr<std::atomic<T> >& shared_ptr_ref() const
    {
        return v;
    }

    std::shared_ptr<std::atomic<T> >& shared_ptr_ref()
    {
        return v;
    }

    const std::shared_ptr<std::atomic<T> >& raw() const
    {
        return shared_ptr_ref();
    }

    std::shared_ptr<std::atomic<T> >& raw()
    {
        return shared_ptr_ref();
    }

    std::atomic<T>& atomic_ref() const
    {
        assert(has_value());
        return (*v);
    }

    T load() const
    {
        return atomic_ref().load();
    }

    T operator*() const
    {
        return load();
    }

    std::atomic<T>& operator+() const
    {
        return atomic_ref();
    }

    const std::shared_ptr<std::atomic<T> >& operator-() const
    {
        return shared_ptr_ref();
    }

    std::shared_ptr<std::atomic<T> >& operator-()
    {
        return shared_ptr_ref();
    }

    bool compare_exchange_weak(T& expected,
                               T val,
                               std::memory_order order = std::memory_order_seq_cst) noexcept
    {
        return (*v).compare_exchange_weak(expected, val, order);
    }
};

template <typename T>
using shared_atomic_ptr = shared_atomic<T*>;

