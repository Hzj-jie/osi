#pragma once

#include <string>
#include "../compiler_self_test_runner.hpp"
#include "../../../compiler/b3style/b3style.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"

namespace osi::compiler::b3style_compiler
{
    class b3style_self_test : public b2style_self_test_runner
    {
    protected:
        bool compile_with_io(primitive::console_io::test_wrapper& io,
                             const std::string& filename,
                             const std::string& content,
                             primitive::simulator& sim) override
        {
            auto guard = b3style::compile_wrapper::with_current_file(filename);
            return b3style::with_functions(primitive::interrupts(io.io())).compile(content, sim);
        }

        bool compile_default(const std::string& filename,
                             const std::string& content,
                             primitive::simulator& sim) override
        {
            auto guard = b3style::compile_wrapper::with_current_file(filename);
            return b3style::with_default_functions().compile(content, sim);
        }

        bool ignore_assertion_case(const std::string& name) const override
        {
            return b2style_self_test_runner::ignore_assertion_case(name) ||
                   name == "define-class-constructor-for-non-class.txt";
        }

    public:
        DEFINE_CASE(b3style_self_test);
    };

    REGISTER_CASE(b3style_self_test);
}
