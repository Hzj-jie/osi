
#pragma once
#include "../../app_info/assert.hpp"
#include "../../threadpool/queue_runner.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/case_wrapper/repeat_case_wrapper.hpp"
#include "../../utt/case_wrapper/multithreading_case_wrapper.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../sync/spin_wait.hpp"
#include <atomic>
#include <array>
#include <stdint.h>

template <template <typename T> class qless>
class queue_runner_case : public icase
{
public:
    const static uint32_t thread_count = 8;
    const static uint32_t repeat_count = 1024;
private:
    queue_runner_t<qless>* q;
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
        q = new queue_runner_t<qless>();
        index.store(0, std::memory_order_relaxed);
        execs.store(0, std::memory_order_relaxed);
        for(auto it = counters.begin(); it != counters.end(); it++)
            (*it).reset();
        return icase::prepare();
    }

    bool execute() override
    {
        assert(q != nullptr);
        uint32_t i = index.fetch_add(1, std::memory_order_consume);
        if(i < counters.size())
        {
            utt_assert.is_true(q->check_push([&, i]()
                                             {
                                                 execs.fetch_add(1, std::memory_order_consume);
                                                 return counters[i].exec();
                                             }));
        }
        return true;
    }

    bool cleanup() override
    {
        std_this_thread_lazy_wait_until(q->empty());
        delete q;
        for(auto it = counters.begin(); it != counters.end(); it++)
            utt_assert.equal((*it).value(), max_value);
        utt_assert.equal<uint32_t>(execs.load(std::memory_order_consume), max_value * counters.size());
        return icase::cleanup();
    }

    DEFINE_CASE(queue_runner_case);
};

template <template <typename T> class qless>
class queue_runner_test : public
          rinne_case_wrapper<
              multithreading_case_wrapper<
                  repeat_case_wrapper<
                      queue_runner_case<qless>,
                      queue_runner_case<qless>::repeat_count + 8>,
                  queue_runner_case<qless>::thread_count>,
              8>
{ };

class queue_runner_qless_test : public queue_runner_test<qless>
{
    DEFINE_CASE(queue_runner_qless_test);
};
REGISTER_CASE(queue_runner_qless_test);

class queue_runner_qless2_test : public queue_runner_test<qless>
{
    DEFINE_CASE(queue_runner_qless2_test);
};
REGISTER_CASE(queue_runner_qless2_test);

class queue_runner_qless3_test : public queue_runner_test<qless>
{
    DEFINE_CASE(queue_runner_qless3_test);
};
REGISTER_CASE(queue_runner_qless3_test);

