
#pragma once
#include <stdint.h>
#include <thread>
#include <chrono>
#include "../envs/processor.hpp"
#include "../envs/nowadays.hpp"
#include "../envs/processor.hpp"

namespace std {
namespace this_thread {
    namespace
    {
        static bool single_yield()
        {
            int64_t n = nowadays.low_res.milliseconds();
            this_thread::yield();
            return (nowadays.low_res.milliseconds() - n > 1);
        }
    }

    static bool yield_weak()
    {
        if(processor.single)
        {
            while(!single_yield());
            return true;
        }
        else
        {
            return single_yield();
        }
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

