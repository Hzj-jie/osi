#pragma once

#include <string>
#include <memory>
#include "../bstyle/bstyle_test.hpp"
#include "../../../compiler/b3style/b3style.hpp"

namespace osi::compiler::b3style_compiler
{
    class b3style_bstyle_test : public bstyle_compiler::bstyle_test_runner
    {
    protected:
        bool parse(primitive::console_io::test_wrapper& io,
                   const std::string& content,
                   primitive::simulator& sim) override
        {
            auto guard = b3style::disable_namespace();
            return b3style::with_functions(primitive::interrupts(io.io())).compile(content, sim);
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

        void test_real__file__()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            {
                auto guard = b3style::parse_wrapper::with_current_file("real__file__.txt");
                utt_assert.is_true(b3style::with_functions(primitive::interrupts(io.io()))
                                       .compile(bstyle_test_data::real__file__, sim));
            }
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("real__file__.txt"));
        }

        bool run() override
        {
            test_nlp_parsable();
            run_bstyle_cases();
            test_real__file__();
            return true;
        }

        DEFINE_CASE(b3style_bstyle_test);
    };

    REGISTER_CASE(b3style_bstyle_test);
}
