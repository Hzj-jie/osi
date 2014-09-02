
#pragma once
#include "../template/singleton.hpp"
#include <memory>
#include <stdint.h>
#include <utility>
#include <functional>
#include "../envs/nowadays.hpp"
#include "../app_info/assert.hpp"
#include "../app_info/error_handle.hpp"
#include "queue_runner.hpp"
#include "../utils/call_stack.hpp"

namespace __stopwatch_private
{
    const static auto& default_timer = nowadays.low_res;
}

const static class stopwatch_t
{
private:
    static auto qr() -> decltype(queue_runner())
    {
        return queue_runner();
    }

    stopwatch_t()
    {
        // BUGBUG in CL / Microsoft (R) C/C++ Optimizing Compiler Version 18.00.21005.1 for x86
        // the static variable in function is not initialized thread-safely
        qr();
    }

public:
    class stopwatch_event
    {
    private:
        const std::function<int64_t(void)> ms_timer;

        const static uint32_t IS_CANCELED = 1;
        const static uint32_t IS_NOT_CANCELED = 0;
        std::atomic<uint32_t> _canceled;

        const static uint32_t IS_NOT_STARTED = 0;
        const static uint32_t IS_BEFORE_CALLBACK = 1;
        const static uint32_t IS_AFTER_CALLBACK = 2;
        std::atomic<uint32_t> _step;

        const uint32_t waitms;
        const std::function<void(void)> d;
        int64_t lastms;

    private:
        static auto cs() -> decltype(call_stack<stopwatch_event*>())
        {
            return call_stack<stopwatch_event*>();
        }

    public:
        bool canceled() const
        {
            return _canceled.load(std::memory_order_consume) == IS_CANCELED;
        }

        bool cancel()
        {
            uint32_t v = IS_NOT_CANCELED;
            return _canceled.compare_exchange_weak(v, IS_CANCELED) &&
                   assert(v == IS_NOT_CANCELED);
        }

        bool resume()
        {
            uint32_t v = IS_CANCELED;
            return _canceled.compare_exchange_weak(v, IS_NOT_CANCELED) &&
                   assert(v == IS_CANCELED);
        }

        bool restart()
        {
            uint32_t v = IS_AFTER_CALLBACK;
            return _step.compare_exchange_weak(v, IS_NOT_STARTED) &&
                   assert(v == IS_AFTER_CALLBACK);
        }

        uint32_t step() const
        {
            return _step.load(std::memory_order_consume);
        }

        bool not_started() const
        {
            return step() == IS_NOT_STARTED;
        }

        bool started() const
        {
            return step() != IS_NOT_STARTED;
        }

        bool before_callback() const
        {
            return step() == IS_BEFORE_CALLBACK;
        }

        bool after_callback() const
        {
            return step() == IS_AFTER_CALLBACK;
        }

        static stopwatch_event& current()
        {
            return (*cs().current());
        }

    private:
        template <typename TIMER>
        stopwatch_event(uint32_t waitms,
                        std::function<void(void)>&& d,
                        const TIMER& in_ms_timer) :
            ms_timer([&in_ms_timer]() { return in_ms_timer.milliseconds(); }),
            _canceled(IS_NOT_CANCELED),
            _step(IS_NOT_STARTED),
            waitms(waitms),
            d(std::move(d)),
            lastms(ms_timer())
        { }

        stopwatch_event(uint32_t waitms, std::function<void(void)>&& d) :
            stopwatch_event(waitms, std::move(d), __stopwatch_private::default_timer)
        { }

        friend class stopwatch_t;

        void mark_before_callback()
        {
            uint32_t v = IS_NOT_STARTED;
            assert(_step.compare_exchange_weak(v, IS_BEFORE_CALLBACK) &&
                   assert(v == IS_NOT_STARTED));
        }

        void mark_after_callback()
        {
            uint32_t v = IS_BEFORE_CALLBACK;
            assert(_step.compare_exchange_weak(v, IS_AFTER_CALLBACK) &&
                   assert(v == IS_BEFORE_CALLBACK));
        }

        bool execute()
        {
            if(canceled()) return false;
            else
            {
                {
                    stopwatch_event* x = this;
                    cs().push(x);
                }
                bool r = false;
                int64_t diff = ms_timer() - lastms - waitms;
                if(diff >= 0)
                {
                    lastms = ms_timer();
                    mark_before_callback();
                    catch_exception(d());
                    mark_after_callback();
                    r = false;
                }
                else r = true;
                cs().pop();
                return r;
            }
        }

    public:
        template <typename TIMER>
        static std::shared_ptr<stopwatch_event> create(uint32_t waitms,
                                                       std::function<void(void)>&& d,
                                                       const TIMER& ms_timer)
        {
            return std::shared_ptr<stopwatch_event>(new stopwatch_event(waitms, std::move(d), ms_timer));
        }

        static std::shared_ptr<stopwatch_event> create(uint32_t waitms,
                                                       std::function<void(void)>&& d)
        {
            return std::shared_ptr<stopwatch_event>(new stopwatch_event(waitms, std::move(d)));
        }
    };
    
    bool push(const std::shared_ptr<stopwatch_event>& e) const
    {
        if(e && !(e->canceled()))
        {
            return assert(qr().check_push([e]()
                                          {
                                              return e->execute();
                                          }));
        }
        else return false;
    }

    template <typename TIMER>
    std::shared_ptr<stopwatch_event> push(uint32_t waitms,
                                          std::function<void(void)>&& d,
                                          const TIMER& ms_timer) const
    {
        std::shared_ptr<stopwatch_event> e = stopwatch_event::create(waitms, std::move(d), ms_timer);
        assert(push(e));
        return e;
    }

    std::shared_ptr<stopwatch_event> push(uint32_t waitms,
                                          std::function<void(void)>&& d) const
    {
        std::shared_ptr<stopwatch_event> e = stopwatch_event::create(waitms, std::move(d));
        assert(push(e));
        return e;
    }

    CONST_SINGLETON(stopwatch_t);
}& stopwatch = stopwatch_t::instance();

