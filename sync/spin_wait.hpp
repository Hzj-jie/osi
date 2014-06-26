
#include <functional>
#include <thread>
#include <stdint.h>
#include "../envs/loops_per_yield.hpp"
using namespace std;

void lazy_wait_when(const function<bool()>& f)
{
    while(f()) this_thread::yield();
}

void strict_wait_when(const function<bool()>& f)
{
    while(f());
}

void wait_when(const function<bool()>& f)
{
    uint32_t i = 0;
    while(f())
    {
        i++;
        if(i > loops_per_yield.count())
        {
        }
    }
}

