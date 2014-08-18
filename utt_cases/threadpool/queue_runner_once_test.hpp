
#pragma once
#include "../../app_info/assert.hpp"
#include "../../app_info/trace.hpp"
#include "../../threadpool/queue_runner_once.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../utt/case_wrapper/repeat_case_wrapper.hpp"
#include "../../formation/concurrent/qless.hpp"
#include "../../formation/concurrent/qless3.hpp"
#include <functional>
#include <boost/predef.h>

template <template <typename T> class Qless>
class queue_runner_once_case : public icase
{
private:
    queue_runner_t<Qless>* q;

    void wait_finish()
    {
        assert(q != nullptr);
        utt_assert.less_or_equal(q->size(), 1, ", ", q->size(), CODE_POSITION());
        q->wait_for_idle();
    }

public:
    uint32_t preserved_processor_count() const override
    {
        return queue_runner_config.thread_count + 1;
    }

    bool prepare() override
    {
        q = new queue_runner_t<Qless>();
        return icase::prepare();
    }

    bool execute() override
    {
        bool c = false;
        utt_assert.is_true(queue_runner_once(*q,
                                             [&]()
                                             {
                                                 utt_assert.is_false(c, CODE_POSITION());
                                                 c = true;
                                             }),
                           CODE_POSITION());
        wait_finish();
        utt_assert.is_true(c, CODE_POSITION());
        return true;
    }

    bool cleanup() override
    {
        delete q;
        return icase::cleanup();
    }

    DEFINE_CASE(queue_runner_once_case);
};

template <template <typename T> class Qless>
class queue_runner_once_test : public
          repeat_case_wrapper<
              queue_runner_once_case<Qless>,
              32>
{
    DEFINE_CASE(queue_runner_once_test);
};

class queue_runner_qless_once_test : public queue_runner_once_test<qless>
{
    DEFINE_CASE(queue_runner_qless_once_test);
};
REGISTER_CASE(queue_runner_qless_once_test);

#if !BOOST_COMP_MSVC // qless = qless2, but what's wrong?
class queue_runner_qless2_once_test : public queue_runner_once_test<qless2>
{
    DEFINE_CASE(queue_runner_qless2_once_test);
};
REGISTER_CASE(queue_runner_qless2_once_test);
#endif

class queue_runner_qless3_once_test : public queue_runner_once_test<qless3>
{
    DEFINE_CASE(queue_runner_qless3_once_test);
};
REGISTER_CASE(queue_runner_qless3_once_test);

