
#pragma once
#include <thread>
#include "../../utils/timing_counter.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../utt/case_wrapper/multithreading_case_wrapper.hpp"
#include "../../utt/case_wrapper/repeat_case_wrapper.hpp"

class timing_counter_case : public icase
{
public:
    bool execute() override
    {
        const uint32_t wait_ms = 100;
        int64_t ms;
        {
            low_res_ms_timing_counter counter(std::ref(ms));
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
        }
        utt_assert.more_or_equal(ms, wait_ms, ", ms == ", ms, ", wait_ms == ", wait_ms);
        utt_assert.less(ms, wait_ms << 1, ", ms == ", ms, ", wait_ms << 1 == ", wait_ms << 1);
        return true;
    }

    DEFINE_CASE(timing_counter_case);
};

class timing_counter_test : public
          multithreading_case_wrapper<
              repeat_case_wrapper<
                  timing_counter_case,
                  1024>,
              8>
{
    DEFINE_CASE(timing_counter_test);
};
REGISTER_CASE(timing_counter_test);

