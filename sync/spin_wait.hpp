
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

    // for test
    inline static void lazy_wait_when_1(const function<bool()>& f) { lazy_wait_when(f); }
    inline static void strict_wait_when_1(const function<bool()>& f) { strict_wait_when(f); }
    inline static void wait_when_1(const function<bool()>& f) { wait_when(f); }

    namespace
    {
        /*
        // inlined in the macro defination
        template <typename T>
        static function<bool()> to_timeout(T&& ms_timer, const function<bool()>& f, uint32_t timeout_ms, bool& r)
        {
            int64_t start_ms = ms_timer.milliseconds();
            return [&, start_ms]()
            {
                r = f();
                return r && ms_timer.milliseconds() - start_ms < timeout_ms;
            };
        }
        */

        const static auto& default_timer = nowadays.low_res;
    }

#define WAIT_WHEN_TEMPLATE(x) \
    template <typename T> \
    static bool x(T&& ms_timer, const function<bool()>& f, uint32_t timeout_ms) { \
        bool r = false; \
        int64_t start_ms = ms_timer.milliseconds(); \
        lazy_wait_when([&]() { \
            r = f(); \
            return r && ms_timer.milliseconds() - start_ms < timeout_ms; }); \
        return !r; } \
    static bool x(const function<bool()>& f, uint32_t timeout_ms) { \
        return x(default_timer, f, timeout_ms); } \
    template <typename T> \
    inline static bool x##_2(T&& ms_timer, const function<bool()>& f, uint32_t timeout_ms) { return x(ms_timer, f, timeout_ms); } \
    inline static bool x##_3(const function<bool()>& f, uint32_t timeout_ms) { return x(f, timeout_ms); }

    WAIT_WHEN_TEMPLATE(lazy_wait_when);
    WAIT_WHEN_TEMPLATE(strict_wait_when);
    WAIT_WHEN_TEMPLATE(wait_when);

#undef WAIT_WHEN_TEMPLATE
} }

