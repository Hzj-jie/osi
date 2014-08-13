
#pragma once
#include "../template/singleton.hpp"
#include "../formation/concurrent/qless.hpp"
#include "../formation/concurrent/qless3.hpp"
#include "../sync/reset_event.hpp"
#include <functional>
#include "../envs/processor.hpp"
#include <algorithm>
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
        thread_count((preenv.queue_runner_thread_count > 0 ?
                      preenv.queue_runner_thread_count :
                      std::max<uint32_t>(processor.count >> 2, 1))),
        busy_wait(preenv.busy_wait),
        interval_ms((preenv.queue_runner_interval_ms > 0 ?
                     preenv.queue_runner_interval_ms :
                     (preenv.interval_ms > 0 ?
                      preenv.interval_ms :
                      __queue_runner_private::interval_ms)))
    { }

    CONST_SINGLETON(queue_runner_config_t);
}& queue_runner_config = queue_runner_config_t::instance();

template <template <typename T> class qless>
class queue_runner_t final
{
public:
    typedef std::function<bool(void)> func_type;
    typedef func_type* func_pointer;

private:
    qless<func_pointer> q;
    auto_reset_event are;
    std::vector<std::thread> threads;
    volatile bool running;

    bool check(func_pointer p)
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

    bool push(func_pointer p)
    {
        if(p == nullptr) return false;
        else
        {
            q.push(p);
            return true;
        }
    }

    bool push(func_type&& p)
    {
        return push(new func_type(std::move(p)));
    }

    bool check_push(func_pointer p)
    {
        if(p == nullptr) return false;
        else
        {
            if(check(p)) assert(push(p));
            return true;
        }
    }

    bool check_push(func_type&& p)
    {
        return check_push(new func_type(std::move(p)));
    }
    
private:
    void work(uint32_t id)
    {
        while(running)
        {
            size_t size = this->size();
            if(queue_runner_config.thread_count > 1)
                size = ceil((double)size / queue_runner_config.thread_count);
            func_pointer p = nullptr;
            while(size > 0 && q.pop(p))
            {
                assert(check_push(p));
                size--;
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

    // public for test purpose
    queue_runner_t() :
        running(true)
    {
        for(uint32_t i = 0; i < queue_runner_config.thread_count; i++)
            threads.push_back(std::thread(&queue_runner_t::work, this, i));
    }

    ~queue_runner_t()
    {
        running = false;
        for(uint32_t i = 0; i < threads.size(); i++)
            threads[i].join();
        func_pointer p = nullptr;
        while(q.pop(p)) delete p;
    }

    SINGLETON(queue_runner_t);
};

template <template <typename T> class qless>
static queue_runner_t<qless>& queue_runner()
{
    return queue_runner_t<qless>::instance();
}

static queue_runner_t<qless>& queue_runner()
{
    return queue_runner<qless>();
}

