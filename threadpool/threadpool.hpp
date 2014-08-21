
#pragma once
#include <stdint.h>
#include <ratio>
#include <algorithm>
#include "../template/singleton.hpp"
#include "../envs/preenv.hpp"
#include "queue_runner.hpp"
#include "../sync/reset_event.hpp"
#include <vector>
#include <thread>
#include <atomic>
#include "../sync/spin_wait.hpp"
#include <chrono>
#include "../app_info/assert.hpp"
#include "../app_info/error_handle.hpp"
#include <utility>

namespace __threadpool_private
{
    const static uint32_t stop_wait_ms = 5000;
}

const static class threadpool_config_t
{
public:
    const bool busy_wait;
    const uint32_t thread_count;
    const uint32_t stop_wait_ms;
private:
    threadpool_config_t() :
        busy_wait(preenv.busy_wait),
        thread_count(preenv.threadpool_thread_count > 0 ?
                     preenv.threadpool_thread_count :
                     (busy_wait ?
                      std::max<uint32_t>(processor.count - queue_runner_config.thread_count, 1) :
                      processor.count)),
        stop_wait_ms(preenv.threadpool_stop_wait_ms > 0 ?
                     preenv.threadpool_stop_wait_ms :
                     __threadpool_private::stop_wait_ms)
    { }

    CONST_SINGLETON(threadpool_config_t);
}& threadpool_config = threadpool_config_t::instance();

template <template <typename T> class Slimqless>
class threadpool_t final
{
private:
    Slimqless<std::function<void(void)>*> q;
    auto_reset_event are;
    std::vector<std::thread> threads;
    volatile bool running;
    std::atomic<uint32_t> working;

    void work(uint32_t id)
    {
        while(true)
        {
            std::function<void(void)>* f = nullptr;
            uint32_t i = 0;
            assert(working.fetch_add(1, std::memory_order_release) < threadpool_config.thread_count);
            while(q.pop(f))
            {
                i++;
                assert(f != nullptr);
                catch_exception((*f)());
                delete f;
            }
            assert(working.fetch_sub(1, std::memory_order_release) > 0);
            if(i == 0 && !running) break;
            else if(!threadpool_config.busy_wait)
                are.wait(threadpool_config.stop_wait_ms);
        }
    }
public:
    // test only
    bool idle() const
    {
        return working.load(std::memory_order_consume) == 0;
    }

    // test only
    void wait_for_idle() const
    {
        // sleep for a while to let working thread to resume
        std::this_thread::sleep_for(std::chrono::milliseconds(256));
        std_this_thread_lazy_wait_until(idle());
    }

    bool push(std::function<void(void)>* p)
    {
        if(p == nullptr) return false;
        else if(q.push(p))
        {
            are.set();
            return true;
        }
        else return false;
    }

    bool push(std::function<void(void)>&& f)
    {
        return push(new std::function<void(void)>(std::move(f)));
    }

    //public for test purpose
    threadpool_t() :
        running(true),
        working(0)
    {
        for(uint32_t i = 0; i < threadpool_config.thread_count; i++)
            threads.push_back(std::thread(&threadpool_t::work, this, i));
    }

    ~threadpool_t()
    {
        running = false;
        for(uint32_t i = 0; i < threads.size(); i++)
            threads[i].join();
    }

    SINGLETON(threadpool_t);
};

template <template <typename T> class Slimqless>
static threadpool_t<Slimqless>& threadpool()
{
    return threadpool_t<Slimqless>::instance();
}

static threadpool_t<slimqless>& threadpool()
{
    return threadpool<slimqless>();
}

