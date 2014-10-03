
#pragma once
#include "../../utils/call_stack.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../utt/fake/construct_counter.hpp"
#include <vector>
#include <stdint.h>

class call_stack_case : public icase
{
private:
    const static call_stack_t<construct_counter<>>& call_stack()
    {
        return ::call_stack<construct_counter<>>();
    }

public:
    bool execute() override
    {
        const int size = __call_stack_private::default_max_size;
        std::vector<uint32_t> indexes;
        for(int i = 0; i < size; i++)
        {
            construct_counter<> c;
            indexes.push_back(c.value);
            call_stack().push(c);
        }
        for(int i = 0; i < size; i++)
        {
            utt_assert.equal(call_stack().current().value, indexes.back());
            call_stack().pop();
            indexes.pop_back();
        }
        return true;
    }

    DEFINE_CASE(call_stack_case);
};

class call_stack_test :
    public multithreading_case_wrapper<
               call_stack_case,
               32>
{
    DEFINE_CASE(call_stack_test);
};
REGISTER_CASE(call_stack_test);

