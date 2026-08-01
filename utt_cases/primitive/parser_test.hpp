#pragma once
#include "../../primitive/parser.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace primitive_test
{
    class parser_data_ref_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_ref ref;
            utt_assert.is_true(parser::parse_data_ref("abs(5)", ref));
            utt_assert.equal(ref, data_ref::abs(5));

            utt_assert.is_true(parser::parse_data_ref("rel10", ref));
            utt_assert.equal(ref, data_ref::rel(10));

            utt_assert.is_true(parser::parse_data_ref("habs(2)", ref));
            utt_assert.equal(ref, data_ref::habs(2));

            return true;
        }

        DEFINE_CASE(parser_data_ref_test);
    };

    class parser_program_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            std::string code = R"(
                # Sample VM Program
                push
                cpc abs(0) i42
                stop
            )";

            std::vector<instruction> insts;
            utt_assert.is_true(parser::parse_program(code, insts));
            utt_assert.equal(insts.size(), static_cast<size_t>(3));
            utt_assert.equal(static_cast<uint8_t>(insts[0].cmd), static_cast<uint8_t>(command_type::push));
            utt_assert.equal(static_cast<uint8_t>(insts[1].cmd), static_cast<uint8_t>(command_type::cpc));
            utt_assert.equal(insts[1].constant_val.as_int32(), 42);

            return true;
        }

        DEFINE_CASE(parser_program_test);
    };

    REGISTER_CASE(parser_data_ref_test);
    REGISTER_CASE(parser_program_test);
}
