#pragma once
#include "executor_case.hpp"

namespace logic_test
{
    class caller_test : public executor_case
    {
    public:
        caller_test()
            : executor_case(
                std::make_shared<osi::compiler::logic::_callee>(
                    "add",
                    "type*",
                    std::make_shared<osi::compiler::logic::paragraph>(
                        std::make_shared<osi::compiler::logic::_define>("result", osi::compiler::logic::scope::type_t::variable_type),
                        std::make_shared<osi::compiler::logic::_add>("result", "parameter1", "parameter2"),
                        std::make_shared<osi::compiler::logic::_return>("add", "result")
                    ),
                    std::vector<osi::compiler::parameter>{
                        {"type*", "parameter1"},
                        {"type*", "parameter2"},
                        {"type*", "parameter3"}
                    }
                ),
                std::make_shared<osi::compiler::logic::_define>("parameter1", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_define>("parameter2", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_define>("parameter3", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_define>("result", osi::compiler::logic::scope::type_t::variable_type),
                std::make_shared<osi::compiler::logic::_copy_const>("parameter1", primitive::data_block(static_cast<uint32_t>(100))),
                std::make_shared<osi::compiler::logic::_copy_const>("parameter2", primitive::data_block(static_cast<uint32_t>(200))),
                std::make_shared<osi::compiler::logic::_copy_const>("parameter3", primitive::data_block(static_cast<uint32_t>(10000))),
                std::make_shared<osi::compiler::logic::_caller>("add", "result", std::vector<std::string>{"parameter1", "parameter2", "parameter3"})
            )
        {
        }

    protected:
        void check_result(primitive::simulator& e) override
        {
            if (utt_assert.equal(e.stack_size(), static_cast<size_t>(4)))
            {
                utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(0)), static_cast<uint32_t>(100));
                utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(1)), static_cast<uint32_t>(200));
                utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(2)), static_cast<uint32_t>(10000));
                utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(3)), static_cast<uint32_t>(300));
            }
        }

        DEFINE_CASE(caller_test);
    };

    REGISTER_CASE(caller_test);
}
