
#pragma once
#include <functional>
#include <thread>
#include <stdint.h>
#include "../envs/loops_per_yield.hpp"
#include "yield.hpp"
#include "../envs/nowadays.hpp"
#include "../delegates/transform.hpp"

namespace std {
namespace this_thread {
    static void lazy_wait_when(const function<bool()>& f)
    {
        while(f()) interval();
    }

    static void slack_wait_when(const function<bool()>& f)
    {
        if(processor.single) lazy_wait_when(f);
        else
        {
            uint32_t lpy = loops_per_yield.count;
            uint32_t i = 0;
            while(f())
            {
                i++;
                if(i >= lpy)
                {
                    interval();
                    i = 0;
                    if(lpy > loops_per_yield.min) lpy >>= 1;
                }
            }
        }
    }

    static void wait_when(const function<bool()>& f)
    {
        if(processor.single) lazy_wait_when(f);
        else
        {
            uint32_t lpy = loops_per_yield.count;
            uint32_t i = 0;
            while(f())
            {
                i++;
                if(i == lpy)
                {
                    if(yield_weak()) i = 0;
                    else i = lpy;
                    if(lpy > loops_per_yield.min) lpy >>= 1;
                }
                else if(i > lpy)
                {
                    yield_strong();
                    i = 0;
                }
            }
        }
    }

    static void busy_wait_when(const function<bool()>& f)
    {
        if(processor.single) lazy_wait_when(f);
        else
        {
            uint32_t lpy = loops_per_yield.count;
            uint32_t i = 0;
            while(f())
            {
                i++;
                if(i >= lpy)
                {
                    if(yield_weak()) i = 0;
                    else i = lpy;
                    if(lpy > loops_per_yield.min) lpy >>= 1;
                }
            }
        }
    }

    static void strict_wait_when(const function<bool()>& f)
    {
        if(processor.single) lazy_wait_when(f);
        else if(processor.few) busy_wait_when(f);
        else while(f());
    }

    static void lazy_wait_until(const function<bool()>& f)
    {
        lazy_wait_when(::reverse(f));
    }

    static void slack_wait_until(const function<bool()>& f)
    {
        slack_wait_when(::reverse(f));
    }

    static void wait_until(const function<bool()>& f)
    {
        wait_when(::reverse(f));
    }

    static void busy_wait_until(const function<bool()>& f)
    {
        busy_wait_when(::reverse(f));
    }

    static void strict_wait_until(const function<bool()>& f)
    {
        strict_wait_when(::reverse(f));
    }

    // for test
    inline static void lazy_wait_when_1(const function<bool()>& f) { lazy_wait_when(f); }
    inline static void slack_wait_when_1(const function<bool()>& f) { slack_wait_when(f); }
    inline static void wait_when_1(const function<bool()>& f) { wait_when(f); }
    inline static void busy_wait_when_1(const function<bool()>& f) { busy_wait_when(f); }
    inline static void strict_wait_when_1(const function<bool()>& f) { strict_wait_when(f); }

    inline static void lazy_wait_until_1(const function<bool()>& f) { lazy_wait_until(f); }
    inline static void slack_wait_until_1(const function<bool()>& f) { slack_wait_until(f); }
    inline static void wait_until_1(const function<bool()>& f) { wait_until(f); }
    inline static void busy_wait_until_1(const function<bool()>& f) { busy_wait_until(f); }
    inline static void strict_wait_until_1(const function<bool()>& f) { strict_wait_until(f); }

    namespace __spin_wait_private
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
        x([&]() { \
            r = f(); \
            return r && ms_timer.milliseconds() - start_ms < timeout_ms; }); \
        return !r; } \
    static bool x(const function<bool()>& f, uint32_t timeout_ms) { \
        return x(__spin_wait_private::default_timer, f, timeout_ms); } \
    template <typename T> \
    inline static bool x##_2(T&& ms_timer, const function<bool()>& f, uint32_t timeout_ms) { \
        return x(ms_timer, f, timeout_ms); } \
    inline static bool x##_3(const function<bool()>& f, uint32_t timeout_ms) { \
        return x(f, timeout_ms); }

    WAIT_WHEN_TEMPLATE(lazy_wait_when);
    WAIT_WHEN_TEMPLATE(slack_wait_when);
    WAIT_WHEN_TEMPLATE(wait_when);
    WAIT_WHEN_TEMPLATE(busy_wait_when);
    WAIT_WHEN_TEMPLATE(strict_wait_when);

#undef WAIT_WHEN_TEMPLATE

#define WAIT_UNTIL_TEMPLATE(x) \
    template <typename T> \
    static bool x(T&& ms_timer, const function<bool()>& f, uint32_t timeout_ms) { \
        bool r = false; \
        int64_t start_ms = ms_timer.milliseconds(); \
        x([&]() { \
            r = f(); \
            return r || ms_timer.milliseconds() - start_ms >= timeout_ms; }); \
        return r; } \
    static bool x(const function<bool()>& f, uint32_t timeout_ms) { \
        return x(__spin_wait_private::default_timer, f, timeout_ms); } \
    template <typename T> \
    inline static bool x##_2(T&& ms_timer, const function<bool()>& f, uint32_t timeout_ms) { \
        return x(ms_timer, f, timeout_ms); } \
    inline static bool x##_3(const function<bool()>& f, uint32_t timeout_ms) { \
        return x(f, timeout_ms); }

    WAIT_UNTIL_TEMPLATE(lazy_wait_until);
    WAIT_UNTIL_TEMPLATE(slack_wait_until);
    WAIT_UNTIL_TEMPLATE(wait_until);
    WAIT_UNTIL_TEMPLATE(busy_wait_until);
    WAIT_UNTIL_TEMPLATE(strict_wait_until);

#undef WAIT_UNTIL_TEMPLATE
} }

