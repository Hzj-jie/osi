
#pragma once
#include <functional>
#include <utility>
#include <stdint.h>
#include "../envs/nowadays.hpp"
#include "../envs/processor.hpp"

template <typename Now>
struct timing_counter
{
private:
    const Now& now;
    const int64_t s;
    std::reference_wrapper<int64_t> p;
public:
    template <typename T>
    timing_counter(T&& p, const Now& now = Now::instance()) :
        now(now),
        s(now()),
        p(std::forward<T>(p)) { }

    ~timing_counter()
    {
        p.get() = (now() - s);
    }
};

struct high_res_ms_timing_counter : public timing_counter<nowadays_t::high_res_milliseconds_t>
{
    template <typename T>
    high_res_ms_timing_counter(T&& p) :
        timing_counter<nowadays_t::high_res_milliseconds_t>(p) { }
};

using ms_timing_counter = high_res_ms_timing_counter;

struct low_res_ms_timing_counter : public timing_counter<nowadays_t::low_res_milliseconds_t>
{
    template <typename T>
    low_res_ms_timing_counter(T&& p) :
        timing_counter<nowadays_t::low_res_milliseconds_t>(p) { }
};

struct processor_loops_counter : public timing_counter<processor_t::processor_loops_t>
{
    template <typename T>
    processor_loops_counter(T&& p) :
        timing_counter<processor_t::processor_loops_t>(p) { }
};

