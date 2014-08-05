
#pragma once
#include "../../sync/exclusive_container.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/case_wrapper/multithreading_case_wrapper.hpp"
#include "../../utt/utt_assert.hpp"
#include <stdint.h>

class exclusive_container_case : public icase
{
public:
    const static uint32_t thread_count = 8;
    const static uint32_t repeat_count = 1024;
private:
    exclusive_container<std::atomic<uint32_t>> c;
public:
    exclusive_container_case()
    {
        c.create();
    }

    bool prepare() override
    {
        utt_assert.is_true(c.clear());
        return true;
    }

    bool execute() override
    {
        c.create();
        if(utt_assert.is_true(c.has_value()))
            c.ref()++;
        else return false;
        return true;
    }

    bool cleanup() override
    {
        utt_assert.equal((*c).load(),
                         thread_count * repeat_count,
                         ", ",
                         (*c).load(),
                         ", ",
                         thread_count * repeat_count);
        return true;
    }

    DEFINE_CASE(exclusive_container_case);
};

class exclusive_container_test :
    public rinne_case_wrapper<
                multithreading_case_wrapper<
                    repeat_case_wrapper<exclusive_container_case,
                                        exclusive_container_case::repeat_count>,
                    exclusive_container_case::thread_count>,
                1024>
{
    DEFINE_CASE(exclusive_container_test);
};

REGISTER_CASE(exclusive_container_test);

