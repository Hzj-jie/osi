
#pragma once
#include <stdint.h>
#include <thread>

namespace __processor_private
{
    const uint32_t few_processor_threshold = 2;
}

class processor_t
{
public:
    const uint32_t count;
    const bool single;
    const bool few;
    processor_t() :
        count(std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency()),
        single(count == 1),
        few(count <= __processor_private::few_processor_threshold) { }
} processor;

