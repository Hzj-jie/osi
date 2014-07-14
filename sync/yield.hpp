
#include <stdint.h>
#include <thread>
#include "../envs/processor.hpp"
#include "../envs/nowadays.hpp"
using namespace std;

namespace
{
    static bool _yield(bool force = true)
    {
        if(force || should_yield())
        {
            int64_t n = nowadays.low_res.milliseconds();
            this_thread::yield();
            return (nowadays.low_res.milliseconds() - n > 1);
        }
        else return false;
    }
}

static bool should_yield()
{
    return processor.single();
}

static bool force_yield()
{
    return _yield(true);
}

static bool not_force_yield()
{
    return _yield(false);
}

