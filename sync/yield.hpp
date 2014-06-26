
#include <thread>
#include "../envs/processor.hpp"
using namespace std;

namespace
{
    bool _yield(bool force = true)
    {
        if(force || should_yield())
        {
            // TODO: not finished
        }
        else return false;
    }
}

bool should_yield()
{
    return processor.single();
}

bool force_yield()
{
    return _yield(true);
}

bool not_force_yield()
{
    return _yield(false);
}

