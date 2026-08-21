#pragma once
#include "executor_case.hpp"
#include <string>

namespace logic_test
{
    class interrupt_test : public executor_case
    {
    private:
        std::string text_;
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;
        std::shared_ptr<primitive::interrupts> interrupts_;

    public:
        interrupt_test()
            : text_("Hello, World! Interrupt testing string with special chars 12345!"),
              io_wrapper_(std::make_shared<primitive::console_io::test_wrapper>(text_)),
              interrupts_(std::make_shared<primitive::interrupts>(io_wrapper_->io()))
        {
            es_builder_ = [this]() {
                return std::vector<std::shared_ptr<osi::compiler::logic::instruction_gen>>{
                    std::make_shared<osi::compiler::logic::_define>("1", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_copy_const>("1", primitive::data_block(static_cast<uint32_t>(1))),
                    std::make_shared<osi::compiler::logic::_define>("i", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_define>("input", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_interrupt>("stdin", "i", "input"),
                    std::make_shared<osi::compiler::logic::_interrupt>("stdout", "input", "i"),
                    std::make_shared<osi::compiler::logic::_interrupt>("stderr", "input", "i"),
                    std::make_shared<osi::compiler::logic::_define>("len", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_sizeof>("len", "input"),
                    std::make_shared<osi::compiler::logic::_define>("i-less-then-len", osi::compiler::logic::scope::type_t::variable_type),
                    std::make_shared<osi::compiler::logic::_do_while>(
                        "i-less-then-len",
                        std::make_shared<osi::compiler::logic::paragraph>(
                            std::make_shared<osi::compiler::logic::_define>("char", osi::compiler::logic::scope::type_t::variable_type),
                            std::make_shared<osi::compiler::logic::_define>("result", osi::compiler::logic::scope::type_t::variable_type),
                            std::make_shared<osi::compiler::logic::_cut_len>("char", "input", "i", "1"),
                            std::make_shared<osi::compiler::logic::_interrupt>("stdout", "char", "result"),
                            std::make_shared<osi::compiler::logic::_interrupt>("stderr", "char", "result"),
                            std::make_shared<osi::compiler::logic::_add>("i", "i", "1"),
                            std::make_shared<osi::compiler::logic::_less>("i-less-then-len", "i", "len")
                        )
                    )
                };
            };
        }

    protected:
        std::shared_ptr<primitive::interrupts> interrupts() override
        {
            return interrupts_;
        }

        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output(), text_ + text_);
            utt_assert.equal(io_wrapper_->error(), text_ + text_);
        }

        DEFINE_CASE(interrupt_test);
    };

    REGISTER_CASE(interrupt_test);
}
