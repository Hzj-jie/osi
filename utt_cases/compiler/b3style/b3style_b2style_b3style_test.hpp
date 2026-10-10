#pragma once

#include <string>
#include <memory>
#include "../b2style/b2style_test.hpp"
#include "../../../compiler/b3style/b3style.hpp"

namespace osi::compiler::b3style_compiler
{
    class b3style_b2style_b3style_test : public b2style_compiler::b2style_test_runner
    {
    protected:
        bool parse(primitive::console_io::test_wrapper& io,
                   const std::string& content,
                   primitive::simulator& sim) override
        {
            return b2style_compiler::compile_wrapper_b3style(primitive::interrupts(io.io())).compile(content, sim);
        }

        bool parse_error_case(const std::string& content) override
        {
            primitive::simulator sim;
            return b2style_compiler::compile_wrapper_b3style(primitive::interrupts()).compile(content, sim);
        }

    public:
        void test_nlp_parsable()
        {
            std::shared_ptr<osi::automata::nlp> parser;
            bool ok = osi::automata::nlp::of_file(b3style::nlexer_rule(),
                                                  b3style::syntaxer_rule(),
                                                  parser);
            utt_assert.is_true(ok);
            utt_assert.is_true(parser != nullptr);
        }

        bool run() override
        {
            test_nlp_parsable();
            run_common_compile_errors();
            run_b2style_cases();
            return true;
        }

        DEFINE_CASE(b3style_b2style_b3style_test);
    };

    REGISTER_CASE(b3style_b2style_b3style_test);
}
