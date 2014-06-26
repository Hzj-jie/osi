
#include <stdint.h>
#include "processor.hpp"

class loops_per_yield_t
{
private:
    uint32_t _count;
public:
    loops_per_yield_t()
    {
        // TODO: update count based on the performance of the system
        // i.e. move system_perf
        _count = (processor.single() ? 1 : 500);
    }

    uint32_t count() const
    {
        return _count;
    }
} loops_per_yield;

