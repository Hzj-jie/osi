
#pragma once
#include <functional>
#include <utility>
#include "../app_info/assert.hpp"
#include "../app_info/error_handle.hpp"
#include "queue_runner.hpp"
#include "../template/singleton.hpp"
#include <stdint.h>
#include "../envs/nowadays.hpp"

namespace __queue_runner_repeat_private
{
    const static auto& default_timer = nowadays.low_res;
}

const static class queue_runner_repeat_t
{
private:
    const static uint32_t NO_TIMEOUT = UINT32_MAX;

    template <template <typename T> class qless>
    class check_queuer : public queue_runner_t<qless>::queuer
    {
    protected:
        const std::function<bool(void)> f;
        const std::function<void(void)> cb;
        const std::function<int64_t(void)> now;
        const int64_t till;
    public:
        template <typename TIMER>
        check_queuer(uint32_t timeout_ms,
                     std::function<bool(void)>&& f,
                     std::function<void(void)>&& cb,
                     TIMER&& ms_timer) :
            f(std::move(f)),
            cb(std::move(cb)),
            now(timeout_ms == NO_TIMEOUT ?
                std::function<int64_t(void)>([]()
                                             {
                                                 return 0;
                                             }) :
                std::function<int64_t(void)>([&ms_timer]()
                                             {
                                                 return ms_timer.milliseconds();
                                             })),
            till(timeout_ms == NO_TIMEOUT ?
                 INT64_MAX :
                 ms_timer.milliseconds() + timeout_ms) { }
    };

    template <template <typename T> class qless>
    class until_queuer : public check_queuer<qless>
    {
    private:
        typedef check_queuer<qless> base;
    public:
        template <typename TIMER>
        until_queuer(uint32_t timeout_ms,
                     std::function<bool(void)>&& f,
                     std::function<void(void)>&& cb,
                     TIMER&& ms_timer) :
            check_queuer<qless>(timeout_ms,
                                std::move(f),
                                std::move(cb),
                                ms_timer) { }

        bool operator()() override
        {
            bool r = true;
            catch_exception(r = base::f());
            if(r || base::now() >= base::till)
            {
                base::cb();
                return false;
            }
            else return true;
        }
    };

    template <template <typename T> class qless>
    class when_queuer : public check_queuer<qless>
    {
    private:
        typedef check_queuer<qless> base;
    public:
        template <typename TIMER>
        when_queuer(uint32_t timeout_ms,
                    std::function<bool(void)>&& f,
                    std::function<void(void)>&& cb,
                    TIMER&& ms_timer) :
            check_queuer<qless>(timeout_ms,
                                std::move(f),
                                std::move(cb),
                                ms_timer) { }

        bool operator()() override
        {
            bool r = false;
            catch_exception(r = base::f());
            if(r && base::now() < base::till) return true;
            else
            {
                base::cb();
                return false;
            }
        }
    };

    queue_runner_repeat_t() { }

public:
    template <template <typename T> class qless, typename TIMER>
    bool until(queue_runner_t<qless>& q,
               std::function<bool(void)>&& f,
               std::function<void(void)>&& cb,
               uint32_t timeout_ms,
               TIMER&& ms_timer) const
    {
        return q.check_push(new until_queuer<qless>(timeout_ms,
                                                    std::move(f),
                                                    std::move(cb),
                                                    std::forward<TIMER>(ms_timer)));
    }

    template <typename TIMER>
    bool until(std::function<bool(void)>&& f,
               std::function<void(void)>&& cb,
               uint32_t timeout_ms,
               TIMER&& ms_timer) const
    {
        return until(queue_runner(),
                     std::move(f),
                     std::move(cb),
                     timeout_ms,
                     std::forward<TIMER>(ms_timer));
    }

    template <template <typename T> class qless>
    bool until(queue_runner_t<qless>& q,
               std::function<bool(void)>&& f,
               std::function<void(void)>&& cb,
               uint32_t timeout_ms) const
    {
        return until(q,
                     std::move(f),
                     std::move(cb),
                     timeout_ms,
                     __queue_runner_repeat_private::default_timer);
    }

    bool until(std::function<bool(void)>&& f,
               std::function<void(void)>&& cb,
               uint32_t timeout_ms) const
    {
        return until(queue_runner(),
                     std::move(f),
                     std::move(cb),
                     timeout_ms);
    }

    template <template <typename T> class qless>
    bool until(queue_runner_t<qless>& q,
               std::function<bool(void)>&& f,
               std::function<void(void)>&& cb) const
    {
        return until(q,
                     std::move(f),
                     std::move(cb),
                     NO_TIMEOUT);
    }

    bool until(std::function<bool(void)>&& f,
               std::function<void(void)>&& cb) const
    {
        return until(queue_runner(),
                     std::move(f),
                     std::move(cb));
    }

    template <template <typename T> class qless, typename TIMER>
    bool when(queue_runner_t<qless>& q,
              std::function<bool(void)>&& f,
              std::function<void(void)>&& cb,
              uint32_t timeout_ms,
              TIMER&& ms_timer) const
    {
        return q.check_push(new when_queuer<qless>(timeout_ms,
                                                   std::move(f),
                                                   std::move(cb),
                                                   std::forward<TIMER>(ms_timer)));
    }

    template <typename TIMER>
    bool when(std::function<bool(void)>&& f,
              std::function<void(void)>&& cb,
              uint32_t timeout_ms,
              TIMER&& ms_timer) const
    {
        return when(queue_runner(),
                    std::move(f),
                    std::move(cb),
                    timeout_ms,
                    std::forward<TIMER>(ms_timer));
    }

    template <template <typename T> class qless>
    bool when(queue_runner_t<qless>& q,
              std::function<bool(void)>&& f,
              std::function<void(void)>&& cb,
              uint32_t timeout_ms) const
    {
        return when(q,
                    std::move(f),
                    std::move(cb),
                    timeout_ms,
                    __queue_runner_repeat_private::default_timer);
    }

    bool when(std::function<bool(void)>&& f,
              std::function<void(void)>&& cb,
              uint32_t timeout_ms) const
    {
        return when(queue_runner(),
                    std::move(f),
                    std::move(cb),
                    timeout_ms);
    }

    template <template <typename T> class qless>
    bool when(queue_runner_t<qless>& q,
              std::function<bool(void)>&& f,
              std::function<void(void)>&& cb) const
    {
        return when(q,
                    std::move(f),
                    std::move(cb),
                    NO_TIMEOUT);
    }

    bool when(std::function<bool(void)>&& f,
               std::function<void(void)>&& cb) const
    {
        return when(queue_runner(),
                    std::move(f),
                    std::move(cb));
    }

    CONST_SINGLETON(queue_runner_repeat_t);
}& queue_runner_repeat = queue_runner_repeat_t::instance();
