
#pragma once
#include <atomic>
#include <string>
#include <stdint.h>
#include <array>
#include "../../../app_info/trace.hpp"
#include "../../../app_info/assert.hpp"
#include "../../../utils/random.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/case_wrapper/multithreading_case_wrapper.hpp"
#include "../../../utt/case_wrapper/repeat_case_wrapper.hpp"
#include "../../../formation/concurrent/slimqless.hpp"

class slimqless_case : public icase
{
public:
    const static int thread_count = 8;
    const static int repeat_count = 131072;

private:
    const static int min_value = 0;
    const static int max_value = min_value + thread_count * repeat_count;
    slimqless<int> q;
    std::array<bool, max_value - min_value> b;
    std::atomic<int> c;

    bool pop_check()
    {
        int v;
        if(q.pop(v))
        {
            utt_assert.is_true(b[v], CODE_POSITION(), "v == ", v);
            b[v] = false;
            return true;
        }
        else return false;
    }

public:
    slimqless_case() :
        q(),
        b(),
        c(min_value)
    {
        b.fill(false);
    }

    bool execute() override
    {
        if(rnd_bool())
        {
            int v = c.fetch_add(1);
            assert(v >= min_value);
            assert(v < max_value);
            assert(!b[v]);
            b[v] = true;
            q.push(v);
        }
        else pop_check();
        return true;
    }

    bool cleanup() override
    {
        assert(c <= max_value);
        while(pop_check());
        for(int i = min_value; i < c; i++)
            utt_assert.is_false(b[i], CODE_POSITION(), "v == ", i);
        for(int i = c; i < max_value; i++)
            assert(!b[i]);
        return true;
    }

    DEFINE_CASE(slimqless_case);
};

class slimqless_test :
    public multithreading_case_wrapper<
               repeat_case_wrapper<
                   slimqless_case,
                   slimqless_case::repeat_count>,
               slimqless_case::thread_count>
{
    DEFINE_CASE(slimqless_test);
};

REGISTER_CASE(slimqless_test);

