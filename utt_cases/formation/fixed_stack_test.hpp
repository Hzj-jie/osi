
#pragma once
#include "../../formation/fixed_stack.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <string>
#include <stdlib.h>
#include "../../utils/random.hpp"

class fixed_stack_test : public icase
{
private:
    bool full_test() const
    {
        const size_t MAX_SIZE = 100;
        fixed_stack<int, MAX_SIZE> s;
        utt_assert.is_true(s.empty());
        for(size_t i = 0; i < MAX_SIZE; i++)
        {
            utt_assert.is_false(s.full());
            utt_assert.is_true(rnd_bool() ? s.push((int)i) : s.emplace((int)i));
            utt_assert.is_false(s.empty());
        }
        utt_assert.is_true(s.full());
        utt_assert.is_false(s.push(0));
        return true;
    }

    template <typename T>
    bool type_test() const { return true; }

public:
    bool run() override
    {
        // TODO: other fixed_stack tests
        return full_test();
    }

    DEFINE_CASE(fixed_stack_test);
};
REGISTER_CASE(fixed_stack_test);

