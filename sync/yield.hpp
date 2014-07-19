
#pragma once
#include <stdint.h>
#include <thread>
#include "../const/threading.hpp"
#include "../envs/processor.hpp"
#include "../envs/nowadays.hpp"
#include "../envs/processor.hpp"

namespace std {
namespace this_thread {
    namespace __yield_private
    {
        static bool single_yield()
        {
            int64_t n = nowadays.low_res.milliseconds();
            this_thread::yield();
            return (nowadays.low_res.milliseconds() - n > 1);
        }
    }

    static void interval()
    {
        sleep_for(thread_interval);
    }

    static void yield_strong()
    {
        using namespace __yield_private;
        if(!single_yield()) interval();
    }

    static bool yield_weak()
    {
        using namespace __yield_private;
		if(processor.single)
        {
            yield_strong();
            return true;
        }
        else return single_yield();
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

