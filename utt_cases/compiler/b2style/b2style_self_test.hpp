#pragma once

#include <string>
#include "../compiler_self_test_runner.hpp"
#include "../../../compiler/b2style/b2style.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"

namespace osi::compiler::b2style_compiler
{
    class b2style_self_test : public b2style_self_test_runner
    {
    protected:
        bool compile_with_io(primitive::console_io::test_wrapper& io,
                             const std::string& filename,
                             const std::string& content,
                             primitive::simulator& sim) override
        {
            auto guard = b2style::compile_wrapper::with_current_file(filename);
            return b2style::with_functions(primitive::interrupts(io.io())).compile(content, sim);
        }

        bool compile_default(const std::string& filename,
                             const std::string& content,
                             primitive::simulator& sim) override
        {
            auto guard = b2style::compile_wrapper::with_current_file(filename);
            return b2style::with_default_functions().compile(content, sim);
        }

    public:
        DEFINE_CASE(b2style_self_test);
    };

    REGISTER_CASE(b2style_self_test);
}
