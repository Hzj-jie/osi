
#pragma once
#include "../../app_info/assert.hpp"
#include "../../threadpool/queue_runner.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/case_wrapper/repeat_case_wrapper.hpp"
#include "../../utt/case_wrapper/multithreading_case_wrapper.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../sync/spin_wait.hpp"
#include "../../formation/concurrent/qless.hpp"
#include "../../formation/concurrent/qless3.hpp"
#include <atomic>
#include <array>
#include <stdint.h>
#include <boost/predef.h>

template <template <typename T> class Qless>
class queue_runner_case : public icase
{
public:
    const static uint32_t thread_count = 8;
    const static uint32_t repeat_count = 1024;
private:
    queue_runner_t<Qless>* q;
    const static int max_value = 100;

    class counter
    {
    private:
        int i;

    public:
        int value() const
        {
            return i;
        }

        bool exec()
        {
            i++;
            return i < max_value;
        }

        void reset()
        {
            i = 0;
        }
    };

    std::array<counter, thread_count * repeat_count> counters;
    std::atomic<uint32_t> index;
    std::atomic<uint32_t> execs;

public:
    bool prepare() override
    {
        q = new queue_runner_t<Qless>();
        index.store(0, std::memory_order_relaxed);
        execs.store(0, std::memory_order_relaxed);
        for(auto it = counters.begin(); it != counters.end(); it++)
            (*it).reset();
        return icase::prepare();
    }

    bool execute() override
    {
        assert(q != nullptr);
        uint32_t i = index.fetch_add(1, std::memory_order_relaxed);
        if(i < counters.size())
        {
            utt_assert.is_true(q->check_push([&, i]()
                                             {
                                                 execs.fetch_add(1, std::memory_order_release);
                                                 return counters[i].exec();
                                             }));
        }
        return true;
    }

    bool cleanup() override
    {
        q->wait_for_idle();
        delete q;
        for(auto it = counters.begin(); it != counters.end(); it++)
            utt_assert.equal((*it).value(), max_value);
        utt_assert.equal<uint32_t>(execs.load(std::memory_order_consume), max_value * counters.size());
        return icase::cleanup();
    }

    DEFINE_CASE(queue_runner_case);
};

template <template <typename T> class Qless>
class queue_runner_test : public
          rinne_case_wrapper<
              multithreading_case_wrapper<
                  repeat_case_wrapper<
                      queue_runner_case<Qless>,
                      queue_runner_case<Qless>::repeat_count + 8>,
                  queue_runner_case<Qless>::thread_count>,
              8>
{ };

class queue_runner_qless_test : public queue_runner_test<qless>
{
    DEFINE_CASE(queue_runner_qless_test);
};
REGISTER_CASE(queue_runner_qless_test);

#if !BOOST_COMP_MSVC // qless = qless2, but what's wrong?
class queue_runner_qless2_test : public queue_runner_test<qless2>
{
    DEFINE_CASE(queue_runner_qless2_test);
};
REGISTER_CASE(queue_runner_qless2_test);
#endif

class queue_runner_qless3_test : public queue_runner_test<qless3>
{
    DEFINE_CASE(queue_runner_qless3_test);
};
REGISTER_CASE(queue_runner_qless3_test);

