#pragma once
#include <cstdint>

namespace primitive
{
    struct registers
    {
        uint64_t ip{0};
        bool carry_over{false};
        bool divided_by_zero{false};
        bool imaginary_number{false};
        bool do_not_advance_ip{false};
        bool halt{false};
        bool stop{false};

        void reset_instruction_flags()
        {
            do_not_advance_ip = false;
        }

        void reset_all()
        {
            ip = 0;
            carry_over = false;
            divided_by_zero = false;
            imaginary_number = false;
            do_not_advance_ip = false;
            halt = false;
            stop = false;
        }
    };
}
