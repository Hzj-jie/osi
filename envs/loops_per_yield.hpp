
#pragma once
#include <stdint.h>
#include "processor.hpp"

class loops_per_yield_t
{
public:
    const uint32_t count;
    // TODO: update count based on the performance of the system
    loops_per_yield_t() :
        count(processor.single ? 1 : 500) { }
} loops_per_yield;

