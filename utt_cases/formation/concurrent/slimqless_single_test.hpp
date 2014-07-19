
#pragma once
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"
#include "../../../formation/concurrent/slimqless.hpp"
#include "../../../utils/strutils.hpp"
#include <string>

class slimqless_single_test : public icase
{
private:
    bool int_case()
    {
        const int MAX = 10000;
        slimqless<int> q;
        for(int i = 0; i < MAX; i++)
            q.push(i);
        for(int i = 0; i < MAX; i++)
        {
            int j = 0;
            utt_assert.is_true(q.pop(j));
            utt_assert.equal(i, j);
        }
        return true;
    }

    bool string_case()
    {
        using namespace std;
        const int MAX = 10000;
        slimqless<string> q;
        for(int i = 0; i < MAX; i++)
            q.push(strcat(i));
        for(int i = 0; i < MAX; i++)
        {
            string j;
            utt_assert.is_true(q.pop(j));
            utt_assert.equal(strcat(i), j);
        }
        return true;
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

