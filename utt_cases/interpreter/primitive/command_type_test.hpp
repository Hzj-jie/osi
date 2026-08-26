#pragma once
#include "../../../interpreter/primitive/command_type.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace primitive_test
{
    class command_type_enum_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            utt_assert.equal(command_type_name(command_type::push), std::string_view("push"));
            utt_assert.equal(command_type_name(command_type::add), std::string_view("add"));
            utt_assert.equal(command_type_name(command_type::jumpif), std::string_view("jumpif"));
            utt_assert.equal(command_type_name(command_type::alloc), std::string_view("alloc"));
            return true;
        }

        DEFINE_CASE(command_type_enum_test);
    };

    REGISTER_CASE(command_type_enum_test);
}
