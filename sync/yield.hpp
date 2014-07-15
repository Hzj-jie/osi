
#pragma once
#include <stdint.h>
#include <thread>
#include <chrono>
#include "../envs/processor.hpp"
#include "../envs/nowadays.hpp"

namespace std {
namespace this_thread {
    static bool yield_weak()
    {
        int64_t n = nowadays.low_res.milliseconds();
        this_thread::yield();
        return (nowadays.low_res.milliseconds() - n > 1);
    }

    static void yield_strong()
    {
        if(!yield_weak())
            sleep_for(chrono::milliseconds(1));
    }

    inline static bool not_force_yield()
    {
        return yield_weak();
    }

    inline static bool weak_yield()
    {
        return yield_weak();
    }

    inline static void force_yield()
    {
        yield_strong();
    }

    inline static void strong_yield()
    {
        yield_strong();
    }
} }

