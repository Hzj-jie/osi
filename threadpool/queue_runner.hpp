
#pragma once
#include "../template/singleton.hpp"
#include "../formation/concurrent/qless.hpp"
#include "../sync/reset_event.hpp"
#include <functional>
#include "../envs/processor.hpp"
#include <algorithm>
#include <vector>
#include <thread>
#include "../envs/preenv.hpp"

namespace __queue_runner_private
{
    const static uint32_t interval_ms = 5;
}

static class queue_runner_t
{
private:
    qless<std::function<bool()>> q;
    auto_reset_event are;
    std::vector<std::thread> threads;
    volatile bool running;

public:
    const uint32_t thread_count;

private:
    static uint32_t determine_thread_count()
    {
        return std::max<uint32_t>(processor.count >> 2, 1);
    }

    void work(uint32_t id)
    {
        while(running)
        {
            size_t size = q.size();
            // TODO: queue_runner
            if(!preenv.busy_wait)
                are.wait(__queue_runner_private::interval_ms);
        }
    }

    void trigger()
    {
        are.set();
    }

    queue_runner_t() :
        running(true),
        thread_count(determine_thread_count())
    {
        for(uint32_t i = 0; i < thread_count; i++)
            threads.push_back(std::thread(&queue_runner_t::work, this, i));
    }

    ~queue_runner_t()
    {
        running = false;
        for(uint32_t i = 0; i < threads.size(); i++)
            threads[i].join();
    }

    SINGLETON(queue_runner_t);
}& queue_runner = queue_runner_t::instance();

