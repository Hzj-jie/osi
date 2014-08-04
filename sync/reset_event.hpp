
#pragma once
#include <thread>
#include <atomic>
#include "../app_info/assert.hpp"
#include <stdint.h>
#include "../envs/nowadays.hpp"
#include "../sync/spin_wait.hpp"

namespace __reset_event_private
{
    const static auto& default_timer = nowadays.low_res;
}

template <bool auto_reset>
class reset_event
{
private:
    const int PASS = 0;
    const int BLOCK = 1;
    std::atomic<int> s;
public:
    reset_event(bool init_pass) :
        s(init_pass ? PASS : BLOCK) { }

    reset_event() :
        reset_event(true) { }

    bool set()
    {
        int v = BLOCK;
        return s.compare_exchange_weak(v, PASS, std::memory_order_consume) &&
               assert(v == BLOCK);
    }

    bool reset()
    {
        int v = PASS;
        return s.compare_exchange_weak(v, BLOCK, std::memory_order_consume) &&
               assert(v == PASS);
    }

    operator bool() const
    {
        return (s.load(std::memory_order_consume) == PASS);
    }

    bool try_wait()
    {
        if(auto_reset)
        {
            int v = PASS;
            return s.compare_exchange_weak(v, BLOCK, std::memory_order_consume) &&
                   assert(v == PASS);
        }
        else
        {
            return s.load(std::memory_order_consume) == PASS;
        }
    }

    template <typename T>
    bool wait(T&& ms_timer, uint32_t timeout_ms)
    {
        return std::this_thread::wait_when(
                        ms_timer,
                        [this]()
                        {
                            return !(this->try_wait());
                        },
                        timeout_ms);
    }

    inline bool wait(uint32_t timeout_ms)
    {
        return wait(__reset_event_private::default_timer, timeout_ms);
    }

    void wait()
    {
        return std::this_thread::wait_when(
                    [this]()
                    {
                        return !(this->try_wait());
                    });
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

