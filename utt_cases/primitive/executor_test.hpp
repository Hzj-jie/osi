#pragma once
#include "../../primitive/simulator.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace primitive_test
{
    class executor_arithmetic_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            simulator sim;
            // push 10, push 20, add (stack[0] = stack[1] + stack[2])
            std::vector<instruction> insts = {
                instruction(command_type::push), // idx 0
                instruction(command_type::push), // idx 1
                instruction(command_type::push), // idx 2
                instruction(command_type::cpc, data_ref::abs(1), data_block::from_int64(10)),
                instruction(command_type::cpc, data_ref::abs(2), data_block::from_int64(20)),
                instruction(command_type::add, {data_ref::abs(0), data_ref::abs(1), data_ref::abs(2)}),
                instruction(command_type::stop)
            };

            sim.load_instructions(insts);
            utt_assert.is_true(sim.run());

            data_block* res = nullptr;
            utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(0), res));
            utt_assert.equal(res->as_int64(), 30LL);

            return true;
        }

        DEFINE_CASE(executor_arithmetic_test);
    };

    class executor_jump_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            simulator sim;
            // 0: push, 1: cpc abs(0)=1, 2: cpc abs(0)=2 (jumped over), 3: stop
            std::vector<instruction> insts = {
                instruction(command_type::push),
                instruction(command_type::cpc, data_ref::abs(0), data_block::from_int64(1)),
                instruction(command_type::jump, {data_ref::abs(4)}),
                instruction(command_type::cpc, data_ref::abs(0), data_block::from_int64(999)),
                instruction(command_type::stop)
            };

            sim.load_instructions(insts);
            utt_assert.is_true(sim.run());

            data_block* res = nullptr;
            utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(0), res));
            utt_assert.equal(res->as_int64(), 1LL);

            return true;
        }

        DEFINE_CASE(executor_jump_test);
    };

    REGISTER_CASE(executor_arithmetic_test);
    REGISTER_CASE(executor_jump_test);
}
