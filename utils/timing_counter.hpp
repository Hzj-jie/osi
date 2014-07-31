
#pragma once
#include <functional>
#include <utility>
#include <stdint.h>
#include "../envs/nowadays.hpp"

template <typename Now>
struct timing_counter
{
private:
    const Now now;
    const int64_t s;
    std::reference_wrapper<int64_t> p;
public:
    template <typename T>
    timing_counter(T&& p) :
        now(Now()),
        s(now()),
        p(std::forward<T>(p)) { }

    ~timing_counter()
    {
        p.get() = (now() - s);
    }
};

namespace __timing_counter_private
{
    struct high_res_milliseconds
    {
        int64_t operator()() const
        {
            return nowadays.high_res.milliseconds();
        }
    };

    struct processor_loops
    {
        int64_t operator()() const
        {
            // TODO: processor times & loops_per_ms
            return nowadays.high_res.nanoseconds();
        }
    };
}

struct ms_timing_counter : public timing_counter<__timing_counter_private::high_res_milliseconds>
{
    template <typename T>
    ms_timing_counter(T&& p) :
        timing_counter<__timing_counter_private::high_res_milliseconds>(p) { }
};

struct processor_loops_counter : public timing_counter<__timing_counter_private::processor_loops>
{
    template <typename T>
    processor_loops_counter(T&& p) :
        timing_counter<__timing_counter_private::processor_loops>(p) { }
};

