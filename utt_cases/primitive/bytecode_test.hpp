#pragma once
#include "../../primitive/bytecode.hpp"
#include "../../primitive/parser.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace primitive_test
{
    class bytecode_serialize_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            std::string code = R"(
                push
                cpc abs(0) s"hello_bytecode"
                stop
            )";

            std::vector<instruction> original;
            utt_assert.is_true(parser::parse_program(code, original));

            std::vector<uint8_t> bytes = bytecode::export_bytes(original);
            utt_assert.is_false(bytes.empty());

            std::vector<instruction> restored;
            utt_assert.is_true(bytecode::import_bytes(bytes, restored));
            utt_assert.equal(restored.size(), original.size());
            utt_assert.equal(restored[1].constant_val.as_string(), std::string("hello_bytecode"));

            return true;
        }

        DEFINE_CASE(bytecode_serialize_test);
    };

    REGISTER_CASE(bytecode_serialize_test);
}
