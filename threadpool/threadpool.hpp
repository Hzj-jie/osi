
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

const static threadpool_config_t
{
public:
    const bool busy_wait;
    const uint32_t thread_count;
private:
    threadpool_config_t() :
        busy_wait(preenv.busy_wait),
        thread_count(preenv.threadpool_thread_count > 0 ?
                     preenv.threadpool_thread_count :
                     (busy_wait ?
                      std::max<uint32_t>(processor.count - queue_runner_config.thread_count, 1) :
                      processor.count))
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
        while(running)
        {
            std::function<void(void)>* f = nullptr;
            while(q.pop(f))
            {
                assert(f != nullptr);
                catch_exception((*f)());
            }

            if(!threadpool_config.busy_wait)
                are.wait();
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
        std::function<void(void)>* p = nullptr;
        while(q.pop(p)) delete p;
    }
};

