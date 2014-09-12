
#pragma once
#include <atomic>
#include <stdint.h>
#include "../../template/singleton.hpp"
#include <boost/config/suffix.hpp>

template <typename DIFFERENCER>
class construct_counter final
{
private:
    SINGLETON_FUNC(std::atomic<uint32_t>, ac, (0));
    SINGLETON_FUNC(std::atomic<uint32_t>, dc, (0));
    SINGLETON_FUNC(std::atomic<uint32_t>, cc, (0));
    SINGLETON_FUNC(std::atomic<uint32_t>, mc, (0));
    SINGLETON_FUNC(std::atomic<uint32_t>, dd, (0));

public:
    const uint32_t index;

    construct_counter() BOOST_NOEXCEPT :
        index(ac().fetch_add(1, std::memory_order_release))
    {
        dc().fetch_add(1, std::memory_order_release);
    }

    construct_counter(const construct_counter&) BOOST_NOEXCEPT :
        index(ac().fetch_add(1, std::memory_order_release))
    {
        cc().fetch_add(1, std::memory_order_release);
    }

    construct_counter(construct_counter&&) BOOST_NOEXCEPT :
        index(ac().fetch_add(1, std::memory_order_release))
    {
        mc().fetch_add(1, std::memory_order_release);
    }

    ~construct_counter() BOOST_NOEXCEPT
    {
        dd().fetch_add(1, std::memory_order_release);
    }

    operator uint32_t() const
    {
        return index;
    }

    uint32_t operator()() const
    {
        return index;
    }

    uint32_t operator*() const
    {
        return operator()();
    }

    static uint32_t default_constructed()
    {
        return dc().load(std::memory_order_consume);
    }

    static uint32_t copy_constructed()
    {
        return cc().load(std::memory_order_consume);
    }

    static uint32_t move_constructed()
    {
        return mc().load(std::memory_order_consume);
    }

    static uint32_t constructed()
    {
        return ac().load(std::memory_order_consume);
    }

    static uint32_t destructed()
    {
        return dd().load(std::memory_order_consume);
    }

    static uint32_t instance_count()
    {
        uint32_t c = constructed();
        uint32_t d = destructed();
        return (c >= d ? c - d : 0);
    }

    static void reset()
    {
        ac().store(0, std::memory_order_release);
        dc().store(0, std::memory_order_release);
        cc().store(0, std::memory_order_release);
        mc().store(0, std::memory_order_release);
        dd().store(0, std::memory_order_release);
    }
};

