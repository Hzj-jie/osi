#pragma once
#include "../../primitive/registers.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace primitive_test
{
    class registers_init_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            registers reg;
            utt_assert.equal(reg.ip, static_cast<uint64_t>(0));
            utt_assert.is_false(reg.carry_over);
            utt_assert.is_false(reg.divided_by_zero);
            utt_assert.is_false(reg.imaginary_number);
            utt_assert.is_false(reg.do_not_advance_ip);
            utt_assert.is_false(reg.halt);
            utt_assert.is_false(reg.stop);
            return true;
        }

        DEFINE_CASE(registers_init_test);
    };

    class registers_reset_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            registers reg;
            reg.ip = 100;
            reg.do_not_advance_ip = true;
            reg.carry_over = true;

            reg.reset_instruction_flags();
            utt_assert.is_false(reg.do_not_advance_ip);
            utt_assert.is_true(reg.carry_over);

            reg.reset_all();
            utt_assert.equal(reg.ip, static_cast<uint64_t>(0));
            utt_assert.is_false(reg.carry_over);
            return true;
        }

        DEFINE_CASE(registers_reset_test);
    };

    REGISTER_CASE(registers_init_test);
    REGISTER_CASE(registers_reset_test);
}
