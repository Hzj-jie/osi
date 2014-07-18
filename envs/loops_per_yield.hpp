
#pragma once
#include <stdint.h>
#include "../template/singleton.hpp"
#include "processor.hpp"

const static class loops_per_yield_t
{
public:
    const uint32_t count;
    // TODO: update count based on the performance of the system
private:
    loops_per_yield_t() :
        count(processor.single ? 1 : 500) { }
    CONST_SINGLETON(loops_per_yield_t);
}& loops_per_yield = loops_per_yield_t::instance();

