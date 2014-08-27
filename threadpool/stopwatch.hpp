
#pragma once
#include "../template/singleton.hpp"
#include <memory>
#include <stdint.h>
#include <utility>
#include <functional>
#include "../envs/nowadays.hpp"
#include "../app_info/assert.hpp"
#include "../app_info/error_handle.hpp"

const static class stopwatch_t
{
private:
    stopwatch_t() = default;

public:
    class stopwatch_event : std::enable_shared_from_this<stopwatch_event>
    {
    private:
        auto& cs = call_stack<stopwatch_event*>();

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
            return cs.current();
        }

    private:
        template <typename TIMER>
        stopwatch_event(uint32_t waitms,
                        std::function<void(void)>&& d,
                        const TIMER& in_ms_timer) :
            ms_timer([&in_ms_timer]() { return in_ms_timer.milliseconds(); })
            _canceled(IS_NOT_CANCELED),
            _step(IS_NOT_STARTED),
            waitms(waitms),
            d(std::move(d)),
            lastms(ms_timer()),
        { }

        stopwatch_event(uint32_t waitms, std::function<void(void)>&& d) :
            stopwatch_event<nowadays_t::low_res_t>(waitms, d, nowadays.low_res)
        { }

        friend class stopwatch;

        void mark_before_callback()
        {
            uint32_t v = IS_NOT_STARTED;
            assert(_step.compare_exchange_weak(v, IS_BEFOER_CALLBACK) &&
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
                cs.push(this);
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
                cs.pop();
                return r;
            }
        }

    public:
        template <typename TIMER>
        static std::shared_ptr<stopwatch_event> create(uint32_t waitms,
                                                       std::function<void(void)>&& d,
                                                       const TIMER& ms_timer)
        {
            return (new stopwatch_event(waitms, std::move(d), ms_timer))->shared_from_this();
        }

        static std::shared_ptr<stopwatch_event> create(uint32_t waitms,
                                                       std::function<void(void)>&& d)
        {
            return (new stopwatch_event(waitms, std::move(d)))->shared_from_this();
        }
    };
    
    bool push(const std::shared_ptr<stopwatch_event>& e)
    {
        if(e || e->canceled())
        {
            return assert(queue_runner().check_push([=e]()
                                                    {
                                                        return e->execute();
                                                    }));
        }
        else return false;
    }

    template <typename TIMER>
    std::shared_ptr<stopwatch_event> push(uint32_t waitms,
                                          std::function<void(void)>&& d,
                                          const TIMER& ms_timer)
    {
        std::shared_ptr<stopwatch_event> e = stopwatch_event::create(waitms, std::move(d), ms_timer);
        assert(push(e));
        return e;
    }

    std::shared_ptr<stopwatch_event> push(uint32_t watims,
                                          std::function<void(void)>&& d)
    {
        std::shared_ptr<stopwatch_event> e = stopwatch_event::create(waitms, std::move(d));
        assert(push(e));
        return e;
    }

    CONST_SINGLETON(stopwatch_t);
}& stopwatch = stopwatch_t::instance();

