#pragma once
#include "../../primitive/instruction.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace primitive_test
{
    class instruction_basic_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            instruction inst1(command_type::add, {data_ref::abs(0), data_ref::rel(0), data_ref::rel(1)});
            utt_assert.equal(static_cast<uint8_t>(inst1.cmd), static_cast<uint8_t>(command_type::add));
            utt_assert.equal(inst1.operands.size(), static_cast<size_t>(3));

            instruction inst2(command_type::cpc, data_ref::rel(0), data_block::from_int64(42));
            utt_assert.equal(static_cast<uint8_t>(inst2.cmd), static_cast<uint8_t>(command_type::cpc));
            utt_assert.equal(inst2.constant_val.as_int64(), 42LL);

            return true;
        }

        DEFINE_CASE(instruction_basic_test);
    };

    REGISTER_CASE(instruction_basic_test);
}
