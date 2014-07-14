
#pragma once
#include <stdint.h>
#include <thread>
using namespace std;

class processor_t
{
public:
    const uint32_t count;
    const bool single;
    processor_t() :
        count(thread::hardware_concurrency() == 0 ? 1 : thread::hardware_concurrency()),
        single(count == 1) { }
} processor;

