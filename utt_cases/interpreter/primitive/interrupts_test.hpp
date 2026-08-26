#pragma once
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace primitive_test
{
    class interrupts_invoke_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            memory_space mem;

            // Custom interrupt handler 99 returning input + "_handled"
            mem.intr().register_handler(99, "custom_test", [](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                std::string s(reinterpret_cast<const char*>(in.data()), in.size());
                s += "_handled";
                return data_block::from_string(s).bytes;
            });

            uint32_t id = 0;
            utt_assert.is_true(mem.intr().get_id("custom_test", id));
            utt_assert.equal(id, 99U);

            auto out = mem.intr().invoke(99, data_block::from_string("test_input").bytes);
            data_block res_db;
            res_db.bytes = out;
            utt_assert.equal(res_db.as_string(), std::string("test_input_handled"));

            return true;
        }

        DEFINE_CASE(interrupts_invoke_test);
    };

    REGISTER_CASE(interrupts_invoke_test);
}
