#pragma once
#include "executor_case.hpp"

namespace logic_test
{
    class while_then_test : public executor_case
    {
    public:
        while_then_test()
            : executor_case(
                std::make_shared<osi::compiler::logic::_define>("state", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_define>("result", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_define>("i", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_define>("1", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_define>("50", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_copy_const>("state", primitive::data_block(true)),
                std::make_shared<osi::compiler::logic::_copy_const>("1", primitive::data_block(static_cast<uint32_t>(1))),
                std::make_shared<osi::compiler::logic::_copy_const>("50", primitive::data_block(static_cast<uint32_t>(50))),
                std::make_shared<osi::compiler::logic::_while_then>(
                    "state",
                    std::make_shared<osi::compiler::logic::paragraph>(
                        std::make_shared<osi::compiler::logic::_add>("i", "i", "1"),
                        std::make_shared<osi::compiler::logic::_add>("result", "result", "i"),
                        std::make_shared<osi::compiler::logic::_less_or_equal>("state", "i", "50")
                    )
                )
            )
        {
        }

    protected:
        void check_result(primitive::simulator& e) override
        {
            utt_assert.equal(e.access_as_bool(primitive::data_ref::abs(0)), false);
            utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(1)), static_cast<uint32_t>(1326));
            utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(2)), static_cast<uint32_t>(51));
            utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(3)), static_cast<uint32_t>(1));
            utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(4)), static_cast<uint32_t>(50));
        }

        DEFINE_CASE(while_then_test);
    };

    REGISTER_CASE(while_then_test);
}
