
#pragma once
#include <semaphore>
#include <atomic>
#include <chrono>
#include "../app_info/assert.hpp"
#include <stdint.h>
#include "../envs/nowadays.hpp"
#include "../sync/spin_wait.hpp"

namespace __reset_event_private
{
    const static auto& default_timer = nowadays.low_res;
}

template <bool auto_reset>
class reset_event;

template <>
class reset_event<true>
{
private:
    std::binary_semaphore sem;
    std::atomic<bool> signaled;

public:
    reset_event(bool init_pass) :
        sem(init_pass ? 1 : 0),
        signaled(init_pass) { }

    reset_event() :
        reset_event(true) { }

    bool set()
    {
        bool expected = false;
        if(signaled.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        {
            assert(!expected);
            sem.release();
            return true;
        }
        return false;
    }

    bool try_wait()
    {
        if(sem.try_acquire())
        {
            signaled.store(false, std::memory_order_release);
            return true;
        }
        return false;
    }

    bool reset()
    {
        return try_wait();
    }

    operator bool() const
    {
        return signaled.load(std::memory_order_acquire);
    }

    template <typename Rep, typename Period>
    bool wait(const std::chrono::duration<Rep, Period>& timeout)
    {
        if(sem.try_acquire_for(timeout))
        {
            signaled.store(false, std::memory_order_release);
            return true;
        }
        return false;
    }

    template <typename T>
    bool wait(T&& ms_timer, uint32_t timeout_ms)
    {
        return std::this_thread::wait_when(
                        ms_timer,
                        [&]()
                        {
                            return !try_wait();
                        },
                        timeout_ms);
    }

    inline bool wait(uint32_t timeout_ms)
    {
        return wait(std::chrono::milliseconds(timeout_ms));
    }

    void wait()
    {
        sem.acquire();
        signaled.store(false, std::memory_order_release);
    }
};

template <>
class reset_event<false>
{
private:
    std::atomic<bool> signaled;

public:
    reset_event(bool init_pass) :
        signaled(init_pass) { }

    reset_event() :
        reset_event(true) { }

    bool set()
    {
        bool expected = false;
        if(signaled.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        {
            assert(!expected);
            signaled.notify_all();
            return true;
        }
        return false;
    }

    bool reset()
    {
        bool expected = true;
        return signaled.compare_exchange_strong(expected, false, std::memory_order_acq_rel) &&
               assert(expected);
    }

    operator bool() const
    {
        return signaled.load(std::memory_order_acquire);
    }

    bool try_wait()
    {
        return signaled.load(std::memory_order_acquire);
    }

    template <typename T>
    bool wait(T&& ms_timer, uint32_t timeout_ms)
    {
        return std::this_thread::wait_when(
                        ms_timer,
                        [&]()
                        {
                            return !try_wait();
                        },
                        timeout_ms);
    }

    inline bool wait(uint32_t timeout_ms)
    {
        return wait(__reset_event_private::default_timer, timeout_ms);
    }

    void wait()
    {
        while(!signaled.load(std::memory_order_acquire))
        {
            signaled.wait(false, std::memory_order_acquire);
        }
    }
};

class auto_reset_event : public reset_event<true>
{
public:
    auto_reset_event(bool init_pass) :
        reset_event(init_pass) { }

    auto_reset_event() :
        reset_event() { }
};

class manual_reset_event : public reset_event<false>
{
public:
    manual_reset_event(bool init_pass) :
        reset_event(init_pass) { }

    manual_reset_event() :
        reset_event() { }
};

