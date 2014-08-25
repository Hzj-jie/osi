
#pragma once
#include "../template/singleton.hpp"
#include <memory>
#include <stdint.h>
#include <utility>
#include <functional>
#include "../envs/nowadays.hpp"

class stopwatch
{
public:
    class stopwatch_event : public std::enable_shared_from_this<stopwatch_event>
    {
    private:
        const static uint32_t IS_CANCELED = 1;
        const static uint32_t IS_NOT_CANCELED = 0;
        uint32_t _canceled;

        const static uint32_t IS_NOT_STARTED = 0;
        const static uint32_t IS_BEFORE_CALLBACK = 1;
        const static uint32_t IS_AFTER_CALLBACK = 2;
        uint32_t _step;

        const uint32_t waitms;
        const std::function<void(void)> d;
        int64_t lastms;

        template <typename TIMER>
        stopwatch_event(uint32_t waitms,
                        std::function<void(void)>&& d,
                        const TIMER& ms_timer) :
            _canceled(IS_NOT_CANCELED),
            _step(IS_NOT_STARTED),
            waitms(waitms),
            d(std::move(d)),
            lastms(ms_timer.milliseconds())
        { }

        stopwatch_event(uint32_t waitms, std::function<void(void)>&& d) :
            stopwatch_event<nowadays_t::low_res_t>(waitms, d, nowadays.low_res)
        { }

        friend class stopwatch;

        bool execute()
        {
        }
    };
};

