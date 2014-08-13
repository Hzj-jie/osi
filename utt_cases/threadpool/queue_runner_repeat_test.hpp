
#pragma once
#include "../../threadpool/queue_runner_repeat.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../sync/spin_wait.hpp"
#include "../../utils/timing_counter.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/trace.hpp"
#include <functional>

class queue_runner_repeat_case : public icase
{
private:
    queue_runner_t<qless>* q;

    void wait_finish()
    {
        assert(q != nullptr);
        utt_assert.less_or_equal(q->size(), 1, ", ", q->size(), CODE_POSITION());
        std_this_thread_wait_until(q->idle());
    }

    bool until_timeout_case()
    {
        assert(q != nullptr);
        const uint32_t timeout_ms = 1000;
        int64_t used_ms;
        bool cbed = false;
        int64_t c = 0;
        {
            ms_timing_counter counter(std::ref(used_ms));
            utt_assert.is_true(queue_runner_repeat.until(*q,
                                                         [&]() { c++; return false; },
                                                         [&]() { cbed = true; },
                                                         timeout_ms),
                               CODE_POSITION());
            wait_finish();
        }
        utt_assert.more_or_equal(used_ms, timeout_ms, ", ", used_ms, CODE_POSITION());
        utt_assert.is_true(cbed, CODE_POSITION());
        utt_assert.more(c, 0, CODE_POSITION());

        c = 0;
        cbed = false;
        {
            ms_timing_counter counter(std::ref(used_ms));
            utt_assert.is_true(queue_runner_repeat.until(*q,
                                                         [&]() { c++; return true; },
                                                         [&]() { cbed = true; },
                                                         timeout_ms),
                               CODE_POSITION());
            wait_finish();
        }
        utt_assert.less(used_ms, timeout_ms, CODE_POSITION());
        utt_assert.is_true(cbed, CODE_POSITION());
        utt_assert.equal(c, 1, ", ", c, CODE_POSITION());

        return true;
    }

    bool when_timeout_case()
    {
        const uint32_t timeout_ms = 1000;
        int64_t used_ms;
        bool cbed = false;
        int64_t c = 0;
        {
            ms_timing_counter counter(std::ref(used_ms));
            utt_assert.is_true(queue_runner_repeat.when(*q,
                                                        [&]() { c++; return true; },
                                                        [&]() { cbed = true; },
                                                        timeout_ms),
                               CODE_POSITION());
            wait_finish();
        }
        utt_assert.more_or_equal(used_ms, timeout_ms, ", ", used_ms, CODE_POSITION());
        utt_assert.is_true(cbed, CODE_POSITION());
        utt_assert.more(c, 0, CODE_POSITION());

        c = 0;
        cbed = false;
        {
            ms_timing_counter counter(std::ref(used_ms));
            utt_assert.is_true(queue_runner_repeat.when(*q,
                                                        [&]() { c++; return false; },
                                                        [&]() { cbed = true; },
                                                        timeout_ms),
                               CODE_POSITION());
            wait_finish();
        }
        utt_assert.less(used_ms, timeout_ms, CODE_POSITION());
        utt_assert.is_true(cbed, CODE_POSITION());
        utt_assert.equal(c, 1, ", ", c, CODE_POSITION());

        return true;
    }

    bool until_case()
    {
        const int64_t count = 1000;
        int64_t c = 0;
        utt_assert.is_true(queue_runner_repeat.until(*q,
                                                     [&]() { c++; return c == count; },
                                                     [&]() { c++; }),
                           CODE_POSITION());
        wait_finish();
        utt_assert.equal(c, count + 1, ", ", c, CODE_POSITION());

        return true;
    }

    bool when_case()
    {
        const int64_t count = 1000;
        int64_t c = 0;
        utt_assert.is_true(queue_runner_repeat.when(*q,
                                                    [&]() { c++; return c < count; },
                                                    [&]() { c++; }),
                           CODE_POSITION());
        wait_finish();
        utt_assert.equal(c, count + 1, ", ", c, CODE_POSITION());

        return true;
    }

public:
    uint32_t preserved_processor_count() const override
    {
        return 2;
    }

    bool prepare() override
    {
        q = new queue_runner_t<qless>();
        return icase::prepare();
    }

    bool execute() override
    {
        return until_timeout_case() &&
               when_timeout_case() &&
               until_case() &&
               when_case();
    }

    bool cleanup() override
    {
        delete q;
        return icase::cleanup();
    }

    DEFINE_CASE(queue_runner_repeat_case);
};

class queue_runner_repeat_test : public
          repeat_case_wrapper<
              queue_runner_repeat_case,
              128>
{
    DEFINE_CASE(queue_runner_repeat_test);
};
REGISTER_CASE(queue_runner_repeat_test);

