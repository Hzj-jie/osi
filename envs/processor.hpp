
#pragma once
#include <stdint.h>
#include <thread>
#include "../template/singleton.hpp"
#include "nowadays.hpp"

namespace __processor_private
{
    const uint32_t few_processor_threshold = 2;
}

const static class processor_t
{
public:
    const uint32_t count;
    const bool single;
    const bool few;

    const class processor_loops_t
    {
    public:
        int64_t operator()() const
        {
            // TODO: processor usage
            return nowadays.high_res.nanoseconds();
        }

    private:
        processor_loops_t() { }
        CONST_SINGLETON(processor_loops_t);
    }& processor_loops = processor_loops_t::instance();

    int64_t loops() const
    {
        return processor_loops_t::instance()();
    }

private:
    processor_t() :
        count(std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency()),
        single(count == 1),
        few(count <= __processor_private::few_processor_threshold) { }
    CONST_SINGLETON(processor_t);
}& processor = processor_t::instance();

