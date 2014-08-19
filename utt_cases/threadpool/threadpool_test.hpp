
#pragma once
#include "../../threadpool/threadpool.hpp"
#include "../../utt/icase.hpp"
#include "../../app_info/trace.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../utt/case_wrapper/multithreading_case_wrapper.hpp"
#include "../../utt/case_wrapper/repeat_case_wrapper.hpp"
#include "../../formation/concurrent/slimqless.hpp"
#include "../../formation/concurrent/slimqless2.hpp"
#include "../../formation/concurrent/qless3.hpp"
#include <atomic>
#include <stdint.h>
#include <boost/predef.h>

template <template <typename T> class Slimqless>
class threadpool_case : public icase
{
public:
    const static uint32_t thread_count = 4;
    const static uint32_t repeat_count = 1024;
private:
    const static uint32_t single_repeat_count = 1024;
    threadpool_t<Slimqless>* tp;
    std::atomic<uint32_t> count;

public:
    bool prepare() override
    {
        tp = new threadpool_t<Slimqless>();
        count.store(0, std::memory_order_release);
        return icase::prepare();
    }

    bool execute() override
    {
        for(uint32_t i = 0; i < single_repeat_count; i++)
        {
            auto& rc = count;
            utt_assert.is_true(tp->push([&rc]()
                                        {
                                            rc.fetch_add(1, std::memory_order_release);
                                        }));
        }
        return true;
    }

    bool cleanup() override
    {
        tp->wait_for_idle();
        delete tp;
        utt_assert.equal(count.load(std::memory_order_consume),
                         thread_count * repeat_count * single_repeat_count,
                         ", ",
                         count.load(std::memory_order_consume),
                         CODE_POSITION());
        return icase::cleanup();
    }

    DEFINE_CASE(threadpool_case);
};

template <template <typename T> class Slimqless>
class threadpool_test : public
          rinne_case_wrapper<
              multithreading_case_wrapper<
                  repeat_case_wrapper<
                      threadpool_case<Slimqless>,
                      threadpool_case<Slimqless>::repeat_count>,
                  threadpool_case<Slimqless>::thread_count>,
          8>
{ };

class threadpool_slimqless_test : public threadpool_test<slimqless>
{
    DEFINE_CASE(threadpool_slimqless_test);
};
REGISTER_CASE(threadpool_slimqless_test);

#if !BOOST_COMP_MSVC // slimqless = slimqless2, but what's wrong?
class threadpool_slimqless2_test : public threadpool_test<slimqless2>
{
    DEFINE_CASE(threadpool_slimqless2_test);
};
REGISTER_CASE(threadpool_slimqless2_test);
#endif

class threadpool_qless3_test : public threadpool_test<qless3>
{
    DEFINE_CASE(threadpool_qless3_test);
};
REGISTER_CASE(threadpool_qless3_test);

