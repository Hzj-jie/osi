
#pragma once
#include <thread>
#include "../../utils/timing_counter.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

class timing_counter_test : public icase
{
public:
    bool run() override
    {
        const uint32_t wait_ms = 100;
        int64_t ms;
        {
            ms_timing_counter counter(std::ref(ms));
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
        }
        utt_assert.more_or_equal(ms, wait_ms, ", ms == ", ms, ", wait_ms == ", wait_ms);
        utt_assert.less(ms, wait_ms << 1, ", ms == ", ms, ", wait_ms << 1 == ", wait_ms << 1);
        return true;
    }

    DEFINE_CASE(timing_counter_test);
};

REGISTER_CASE(timing_counter_test);

