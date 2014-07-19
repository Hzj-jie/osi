
#pragma once
#include <stdint.h>
#include "../template/singleton.hpp"
#include "processor.hpp"

const static class loops_per_yield_t
{
public:
    // TODO: update count based on the performance of the system
    const uint32_t count;
    const uint32_t min;
private:
#undef min
    loops_per_yield_t() :
        count(processor.single ? 1 : 512),
        min(processor.single ? 1 : 64) { }
    CONST_SINGLETON(loops_per_yield_t);
}& loops_per_yield = loops_per_yield_t::instance();

