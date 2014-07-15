
#include <functional>
#include <thread>
#include <stdint.h>
#include "../envs/loops_per_yield.hpp"
#include "yield.hpp"
#include "../envs/nowadays.hpp"

namespace std {
namespace this_thread {
    static void lazy_wait_when(const function<bool()>& f)
    {
        while(f()) this_thread::yield();
    }

    static void strict_wait_when(const function<bool()>& f)
    {
        while(f());
    }

    static void wait_when(const function<bool()>& f)
    {
        uint32_t i = 0;
        while(f())
        {
            i++;
            if(i > loops_per_yield.count)
            {
                if(yield_weak()) i = 0;
                else i = loops_per_yield.count;
            }
        }
    }

    namespace
    {
        template <typename T>
        static function<bool()> to_timeout(T&& ms_timer, const function<bool()>& f, uint32_t timeout_ms, bool& r)
        {
            uint32_t start_ms = ms_timer.milliseconds();
            return [&]()
            {
                r = f();
                return r || ms_timer.milliseconds() - start_ms >= timeout_ms;
            };
        }

        const static auto& default_timer = nowadays.low_res;
    }

#define WAIT_WHEN_TEMPLATE(x) \
    template <typename T> \
    static bool x(T&& ms_timer, const function<bool()>& f, uint32_t timeout_ms) { \
        bool r = false; \
        lazy_wait_when(to_timeout(ms_timer, f, timeout_ms, r)); \
        return r; } \
    static bool x(const function<bool()>& f, uint32_t timeout_ms) { \
        return x(default_timer, f, timeout_ms); }

    WAIT_WHEN_TEMPLATE(lazy_wait_when);
    WAIT_WHEN_TEMPLATE(strict_wait_when);
    WAIT_WHEN_TEMPLATE(wait_when);

#undef WAIT_WHEN_TEMPLATE
} }

