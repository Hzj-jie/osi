
#pragma once
#include "../../threadpool/stopwatch.hpp"
#include "../../utt/icase.hpp"
#include <stdint.h>
#include "../../envs/nowadays.hpp"
#include "../../utt/utt_assert.hpp"
#include <memory>
#include "../../sync/spin_wait.hpp"
#include "../../utt/case_wrapper/repeat_case_wrapper.hpp"
#include "../../utt/case_wrapper/multithreading_case_wrapper.hpp"

class stopwatch_case : public icase
{
public:
    bool prepare() override
    {
        // BUGBUG in CL / Microsoft (R) C/C++ Optimizing Compiler Version 18.00.21005.1 for x86
        // the static variable in function is not initialized thread-safely
        utt_assert.is_true((bool)(stopwatch.push(0,
                                                 []() { })));
        return icase::prepare();
    }

    bool execute() override
    {
        const uint32_t waitms = 100;
        int64_t n = nowadays.low_res.milliseconds();
        bool executed = false;
        decltype(stopwatch.push(waitms, std::function<void(void)>())) e =
            stopwatch.push(waitms,
                           [&]()
                           {
                               std_this_thread_lazy_wait_until(e);
                               utt_assert.equal(&(stopwatch_t::stopwatch_event::current()), e.get());
                               utt_assert.more_or_equal(nowadays.low_res.milliseconds(), n + waitms);
                               utt_assert.less_or_equal(nowadays.low_res.milliseconds(), n + (waitms << 1));
                               executed = true;
                           });
        utt_assert.is_true((bool)e);
        std_this_thread_lazy_wait_until(e->after_callback());
        utt_assert.more_or_equal(nowadays.low_res.milliseconds(), n + waitms);
        utt_assert.less_or_equal(nowadays.low_res.milliseconds(), n + (waitms << 1));
        utt_assert.is_true(executed);
        return true;
    }

    DEFINE_CASE(stopwatch_case);
};

class stopwatch_test : public
          multithreading_case_wrapper<
              repeat_case_wrapper<
                  stopwatch_case,
                  1024>,
              8>
{
    DEFINE_CASE(stopwatch_test);
};
REGISTER_CASE(stopwatch_test);

