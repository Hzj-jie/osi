
#pragma once
#include "../threadpool/threadpool.hpp"
#include "../threadpool/queue_runner.hpp"
#include <mutex>
#include <stdint.h>
#include <functional>
#include <vector>
#include "../app_info/trace.hpp"
#include "../formation/ternary.hpp"
#include "../delegates/event.hpp"
#include "../threadpool/stopwatch.hpp"
#include <memory>
#include <mutex>

class event_comb
{
public:
    using event_comb_type = std::shared_ptr<event_comb>;

private:
    const static int end_step = INT_MAX;
    const static int not_started_step = -1;
    const static int first_step = 0;
    std::vector<std::function<bool(void)>> ds;
    const code_position ctor_position;
    ternary _end_result;
    stopwatch_event timeout_event;
    event_comb_type cb;
    int step;
    uint32_t pends;
    int64_t _begin_ms;
    int64_t _end_ms;
    std::mutex _l;

protected:
    event<> suspending;
};

