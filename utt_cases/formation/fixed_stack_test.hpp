
#pragma once
#include "../../formation/fixed_stack.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <string>
#include <stdlib.h>
#include "../../utils/random.hpp"
#include "../../utils/convertor.hpp"
#include <stdint.h>
#include <utility>

class fixed_stack_test : public icase
{
private:
    template <typename T>
    bool test_case() const
    {
        const size_t MAX_SIZE = 100;
        fixed_stack<T, MAX_SIZE> s;
        utt_assert.is_true(s.empty());
        for(size_t i = 0; i < MAX_SIZE; i++)
        {
            utt_assert.is_false(s.full());
            utt_assert.is_true(rnd_bool() ?
                               s.push(convertor.convert<size_t, T>(i)) :
                               s.emplace(convertor.convert<size_t, T>(i)));
            utt_assert.is_false(s.empty());
        }
        utt_assert.is_true(s.full());
        utt_assert.is_false(s.push(convertor.convert<size_t, T>(0)));
        for(size_t i = MAX_SIZE; i > 0; i--)
        {
            utt_assert.is_false(s.empty());
            utt_assert.equal(convertor.convert<T, size_t>(s.back()), i - 1);
            T o;
            utt_assert.is_true(s.pop(o));
            utt_assert.equal(convertor.convert<T, size_t>(o), i - 1);
            utt_assert.is_false(s.full());
        }
        utt_assert.is_true(s.empty());

        return true;
    }

public:
    bool run() override
    {
        return test_case<int>() &&
               test_case<std::string>() &&
               test_case<uint32_t>();
    }

    DEFINE_CASE(fixed_stack_test);
};
REGISTER_CASE(fixed_stack_test);

