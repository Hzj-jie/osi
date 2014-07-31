
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
#include "../../../utt/case_wrapper/specific_case_wrapper.hpp"

template <template <typename T> class slimqless>
class slimqless_perf_case : public icase
{
private:
    slimqless<int> q;
public:
    bool execute() override
    {
        static atomic<int> count;
        if(count > 1024 * 1024 || rnd_bool())
        {
            int v;
            q.pop(v);
            count--;
        }
        else
        {
            q.push(rnd_int());
            count++;
        }
        return true;
    }

    bool cleanup() override
    {
        q.clear();
        return true;
    }

    DEFINE_CASE(slimqless_perf_case);
};

template <template <typename T> class slimqless, uint32_t thread_count>
class slimqless_perf_test_template :
    public specific_case_wrapper<
               multithreading_case_wrapper<
                   repeat_case_wrapper<
                       slimqless_perf_case<slimqless>,
                       32 * 1024 / thread_count * 1024>,
                   thread_count> > { };

template <template <typename T> class slimqless>
class slimqless_perf_test_8_template :
    public slimqless_perf_test_template<slimqless, 8> { };

template <template <typename T> class slimqless>
class slimqless_perf_test_32_template :
    public slimqless_perf_test_template<slimqless, 32> { };

template <template <typename T> class slimqless>
class slimqless_perf_test_128_template :
    public slimqless_perf_test_template<slimqless, 128> { };

template <template <typename T> class slimqless>
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
        if(rnd_bool() && rnd_bool()) pop_check();
        else
        {
            int v = c.fetch_add(1);
            assert(v >= min_value);
            assert(v < max_value);
            assert(!b[v]);
            b[v] = true;
            q.push(v);
        }
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

template <template <typename T> class slimqless>
class slimqless_test_template :
    public multithreading_case_wrapper<
               repeat_case_wrapper<
                   slimqless_case<slimqless>,
                   slimqless_case<slimqless>::repeat_count>,
               slimqless_case<slimqless>::thread_count> { };


