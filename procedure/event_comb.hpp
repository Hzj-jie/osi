
#pragma once
#include "../threadpool/threadpool.hpp"
#include "../threadpool/queue_runner.hpp"
#include <mutex>
#include <stdint.h>
#include <functional>
#include <vector>
#include "../app_info/trace.hpp"
#include "../formation/ternary.hpp"

class event_comb
{
private:
    const static int end_step = INT_MAX;
    const static int not_started_step = -1;
    const static int first_step = 0;
    std::vector<std::function<bool(void)>> ds;
    const code_position ctor_position;
    ternary _end_result;
};

