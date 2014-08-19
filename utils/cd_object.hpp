
#pragma once
#include <atomic>
#include <utility>
#include <stdint.h>
#include "../template/singleton.hpp"

template <typename T>
class cd_object
{
private:
    SINGLETON_FUNC(std::atomic<uint32_t>, c, (0));
    SINGLETON_FUNC(std::atomic<uint32_t>, d, (0));
    T* v;
public:
    template <typename... Args>
    cd_object(Args&&... args)
    {
        v = new T(std::forward<Args>(args)...);
        c().fetch_add(1, std::memory_order_acq_rel);
    }

    ~cd_object()
    {
        d().fetch_add(1, std::memory_order_acq_rel);
        delete v;
    }

    T* get() const
    {
        return v;
    }

    T& ref() const
    {
        return *(get());
    }

    T* operator->() const
    {
        return get();
    }

    T& operator*() const
    {
        return ref();
    }

    static uint32_t create_times()
    {
        return c().load(std::memory_order_consume);
    }

    static uint32_t destruct_times()
    {
        return d().load(std::memory_order_consume);
    }

    static uint32_t instance_count()
    {
        uint32_t c = create_times();
        uint32_t d = destruct_times();
        return c >= d ? c - d : 0;
    }

    static void reset()
    {
        c() = 0;
        d() = 0;
    }
};

