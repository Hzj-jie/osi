#pragma once
#include "executor_case.hpp"
#include <string>

namespace logic_test
{
    class condition_test : public executor_case
    {
    private:
        std::string value1_;
        std::string value2_;
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;
        std::shared_ptr<primitive::interrupts> ext_;

        static std::shared_ptr<osi::compiler::logic::_if> create_condition()
        {
            return std::make_shared<osi::compiler::logic::_if>(
                "a-bool",
                std::make_shared<osi::compiler::logic::paragraph>(
                    std::make_shared<osi::compiler::logic::_interrupt>("stdout", "value1", "value")
                ),
                std::make_shared<osi::compiler::logic::paragraph>(
                    std::make_shared<osi::compiler::logic::_interrupt>("stdout", "value2", "value")
                )
            );
        }

    public:
        condition_test()
            : value1_("random_val_1_test_line\n"),
              value2_("random_val_2_test_line\n"),
              io_wrapper_(std::make_shared<primitive::console_io::test_wrapper>()),
              ext_(std::make_shared<primitive::interrupts>(io_wrapper_->io()))
        {
            es_builder_ = [this]() {
                return std::vector<std::shared_ptr<osi::compiler::logic::instruction_gen>>{
                    std::make_shared<osi::compiler::logic::_define>("a-bool", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_define>("value1", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_copy_const>("value1", primitive::data_block(value1_)),
                    std::make_shared<osi::compiler::logic::_define>("value2", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_copy_const>("value2", primitive::data_block(value2_)),
                    std::make_shared<osi::compiler::logic::_define>("value", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_copy_const>("a-bool", primitive::data_block(true)),
                    create_condition(),
                    std::make_shared<osi::compiler::logic::_copy_const>("a-bool", primitive::data_block(false)),
                    create_condition(),
                    std::make_shared<osi::compiler::logic::_stop>()
                };
            };
        }

    protected:
        std::shared_ptr<primitive::interrupts> interrupts() override
        {
            return ext_;
        }

        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output(), value1_ + value2_);
        }

        DEFINE_CASE(condition_test);
    };

    REGISTER_CASE(condition_test);
}
