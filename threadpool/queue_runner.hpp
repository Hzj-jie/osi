
#pragma once
#include "../template/singleton.hpp"
#include "../formation/concurrent/qless.hpp"
#include "../formation/concurrent/qless3.hpp"
#include "../sync/reset_event.hpp"
#include <functional>
#include "../envs/processor.hpp"
#include <algorithm>
#include <atomic>
#include <utility>
#include <vector>
#include <thread>
#include "../envs/preenv.hpp"
#include "../app_info/assert.hpp"
#include <math.h>

namespace __queue_runner_private
{
    const static uint32_t interval_ms = 5;
}

const static class queue_runner_config_t
{
public:
    const uint32_t thread_count;
    const bool busy_wait;
    const uint32_t interval_ms;

private:
    queue_runner_config_t() :
        thread_count(preenv.queue_runner_thread_count > 0 ?
                     preenv.queue_runner_thread_count :
                     std::max<uint32_t>(processor.count >> 2, 1)),
        busy_wait(preenv.busy_wait),
        interval_ms((preenv.queue_runner_interval_ms > 0 ?
                     preenv.queue_runner_interval_ms :
                     (preenv.interval_ms > 0 ?
                      preenv.interval_ms :
                      __queue_runner_private::interval_ms)))
    { }

    CONST_SINGLETON(queue_runner_config_t);
}& queue_runner_config = queue_runner_config_t::instance();

template <template <typename T> class Qless>
class queue_runner_t final
{
public:
    class queuer
    {
    public:
        virtual bool operator()() = 0;
        virtual ~queuer() = default;
    };

private:
    class func_queuer : public queuer
    {
    private:
        const std::function<bool(void)> f;
    public:
        func_queuer(std::function<bool(void)>&& f) :
            f(std::move(f)) { }

        bool operator()() override
        {
            return f();
        }
    };

    Qless<queuer*> q;
    auto_reset_event are;
    std::vector<std::thread> threads;
    volatile bool running;
    std::atomic<uint32_t> working;

    bool check(queuer* p)
    {
        assert(p != nullptr);
        bool r = false;
        catch_exception(r = (*p)());
        return r;
    }

public:
    size_t size() const
    {
        return q.size();
    }

    bool empty() const
    {
        return q.empty();
    }

    bool idle() const
    {
        return (working.load(std::memory_order_acquire) == 0) &&
               empty();
    }

    bool push(queuer* p)
    {
        if(p == nullptr) return false;
        else
        {
            q.push(p);
            return true;
        }
    }

    bool push(std::function<bool(void)>&& p)
    {
        return assert(push(new func_queuer(std::move(p))));
    }

    bool check_push(queuer* p)
    {
        if(p == nullptr) return false;
        else
        {
            if(check(p)) assert(push(p));
            return true;
        }
    }

    bool check_push(std::function<bool(void)>&& p)
    {
        return check_push(new func_queuer(std::move(p)));
    }
    
private:
    void work(uint32_t id)
    {
        while(running)
        {
            size_t size = this->size();
            if(size > 0)
            {
                assert(working.fetch_add(1, std::memory_order_release) <
                       queue_runner_config.thread_count);
                if(queue_runner_config.thread_count > 1)
                    size = ceil((double)size / queue_runner_config.thread_count);
                assert(size > 0);
                queuer* p = nullptr;
                while(size > 0 && q.pop(p))
                {
                    assert(check_push(p));
                    size--;
                }
                assert(working.fetch_sub(1, std::memory_order_release) > 0);
            }
            if(!queue_runner_config.busy_wait)
                are.wait(queue_runner_config.interval_ms);
        }
    }

public:
    void trigger()
    {
        are.set();
    }

#define QUEUE_RUNNER_USE_IDLE_OR_TRIGGER 1
#ifdef QUEUE_RUNNER_USE_IDLE_OR_TRIGGER
private:
    bool idle_or_trigger()
    {
        if(idle()) return true;
        else
        {
            trigger();
            return false;
        }
    }
#endif

public:
    void wait_for_idle()
    {
#if QUEUE_RUNNER_USE_IDLE_OR_TRIGGER
        std_this_thread_lazy_wait_until(idle_or_trigger());
#else
        std_this_thread_lazy_wait_until(idle());
#endif
    }

    // public for test purpose
    queue_runner_t() :
        running(true),
        working(0)
    {
        for(uint32_t i = 0; i < queue_runner_config.thread_count; i++)
            threads.push_back(std::thread(&queue_runner_t::work, this, i));
    }

    ~queue_runner_t()
    {
        running = false;
        for(uint32_t i = 0; i < threads.size(); i++)
            threads[i].join();
        queuer* p = nullptr;
        while(q.pop(p)) delete p;
    }

    SINGLETON(queue_runner_t);
};

template <template <typename T> class Qless>
static queue_runner_t<Qless>& queue_runner()
{
    return queue_runner_t<Qless>::instance();
}

static queue_runner_t<qless>& queue_runner()
{
    return queue_runner<qless>();
}

