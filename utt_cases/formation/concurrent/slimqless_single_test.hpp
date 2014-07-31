
#pragma once
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"
#include "../../../formation/concurrent/slimqless.hpp"
#include "../../../utils/strutils.hpp"
#include <string>
#include <functional>

class slimqless_single_test : public icase
{
private:
    template <typename T>
    bool run_case(const std::function<T(int)>& from_int)
    {
        const int MAX = 10000;
        slimqless<T> q;
        for(int i = 0; i < MAX; i++)
            q.push(from_int(i));
        for(int i = 0; i < MAX; i++)
        {
            T j;
            utt_assert.is_false(q.empty());
            utt_assert.is_true(q.pop(j));
            utt_assert.equal(from_int(i), j);
            utt_assert.equal(q.empty(), i == MAX - 1);
        }
        return true;
    }

    bool int_case()
    {
        return run_case<int>(
                        [](int i)
                        {
                            return i;
                        });
    }

    bool string_case()
    {
        return run_case<std::string>(
                       [](int i)
                       {
                           return strcat(i);
                       });
    }

public:
    bool run() override
    {
        return int_case() &&
               string_case();
    }

    DEFINE_CASE(slimqless_single_test);
};

REGISTER_CASE(slimqless_single_test);

