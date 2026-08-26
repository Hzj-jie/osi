#pragma once
#include <vector>
#include "command_type.hpp"
#include "data_ref.hpp"
#include "data_block.hpp"

namespace primitive
{
    struct instruction
    {
        command_type cmd{command_type::unknown};
        std::vector<data_ref> operands;
        data_block constant_val;

        instruction() = default;
        explicit instruction(command_type c) : cmd(c) {}
        instruction(command_type c, data_ref ref)
            : cmd(c), operands{ref} {}
        instruction(command_type c, std::vector<data_ref> ops)
            : cmd(c), operands(std::move(ops)) {}
        instruction(command_type c, data_ref target_ref, data_block const_val)
            : cmd(c), operands{target_ref}, constant_val(std::move(const_val)) {}

        bool operator==(const instruction& other) const
        {
            return cmd == other.cmd &&
                   operands == other.operands &&
                   constant_val == other.constant_val;
        }

        bool operator!=(const instruction& other) const
        {
            return !(*this == other);
        }
    };
}
