#pragma once
#include "../../primitive/simulator.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace primitive_test
{
    class simulator_heap_alloc_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            simulator sim;
            // push stack[0] (size=5), push stack[1] (heap_id target), alloc stack[1], stack[0]
            std::vector<instruction> insts = {
                instruction(command_type::push), // idx 0
                instruction(command_type::push), // idx 1
                instruction(command_type::cpc, data_ref::abs(0), data_block::from_int64(5)),
                instruction(command_type::alloc, {data_ref::abs(1), data_ref::abs(0)}),
                instruction(command_type::cpc, data_ref::habs(1), data_block::from_string("heap_data")),
                instruction(command_type::stop)
            };

            sim.load_instructions(insts);
            utt_assert.is_true(sim.run());

            data_block* heap_val = nullptr;
            utt_assert.is_true(sim.mem().resolve_ref(data_ref::habs(1), heap_val));
            utt_assert.equal(heap_val->as_string(), std::string("heap_data"));

            return true;
        }

        DEFINE_CASE(simulator_heap_alloc_test);
    };

    class simulator_call_stack_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            simulator sim;
            // 0: push, 1: stst, 2: jump 5, 3: cpc abs(0)=100, 4: stop, 5: push, 6: cpc rel(0)=55, 7: rest
            std::vector<instruction> insts = {
                instruction(command_type::push),
                instruction(command_type::stst),
                instruction(command_type::jump, {data_ref::abs(5)}),
                instruction(command_type::cpc, data_ref::abs(0), data_block::from_int64(100)),
                instruction(command_type::stop),
                instruction(command_type::push),
                instruction(command_type::cpc, data_ref::rel(0), data_block::from_int64(55)),
                instruction(command_type::rest)
            };

            sim.load_instructions(insts);
            utt_assert.is_true(sim.run());

            data_block* val = nullptr;
            utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(0), val));
            utt_assert.equal(val->as_int64(), 100LL);

            return true;
        }

        DEFINE_CASE(simulator_call_stack_test);
    };

    REGISTER_CASE(simulator_heap_alloc_test);
    REGISTER_CASE(simulator_call_stack_test);
}
