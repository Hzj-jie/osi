
#pragma once
#include <stdint.h>
#include <thread>

class processor_t
{
public:
    const uint32_t count;
    const bool single;
    processor_t() :
        count(std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency()),
        single(count == 1) { }
} processor;

