#pragma once

#include <iostream>
#include <string>
#include <memory>
#include "bstyle_test_data.hpp"
#include "../../../compiler/bstyle/bstyle.hpp"
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"
#include "../../../interpreter/primitive/console_io.hpp"
#include "../../../automata/nlp.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace osi::compiler::bstyle_compiler
{
    class bstyle_test : public icase
    {
    private:
        static bool parse(primitive::console_io::test_wrapper& io,
                          const std::string& content,
                          primitive::simulator& sim)
        {
            return bstyle::with_functions(primitive::interrupts(io.io())).compile(content, sim);
        }

        void assert_execute_without_errors(primitive::simulator& sim)
        {
            sim.execute();
            if (sim.halt())
            {
                std::cerr << "Execution halted with error: " << sim.halt_error() << std::endl;
            }
            utt_assert.is_false(sim.halt());
        }

    public:
        void test_nlp_parsable()
        {
            std::shared_ptr<osi::automata::nlp> parser;
            bool ok = osi::automata::nlp::of(bstyle::nlexer_rule(),
                                             bstyle::syntaxer_rule(),
                                             parser);
            utt_assert.is_true(ok);
            utt_assert.is_true(parser != nullptr);
        }

        void test_case1()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::case1, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("Hello World"));
        }

        void test_case2()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::case2, sim));
            assert_execute_without_errors(sim);
            std::string expected;
            for (int i = 0; i < 100; ++i)
            {
                expected += "False";
            }
            utt_assert.equal(io.output(), expected);
        }

        void test_global_variable()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::global_variable, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("TrueFalse"));
        }

        void test_overload_function()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::overload_function, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("TrueFalseFalseTrue"));
        }

        void test_single_level_struct()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::single_level_struct, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("dabc"));
        }

        void test_nested_struct()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::nested_struct, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("dd"));
        }

        void test_function_struct()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::function_struct, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("abcdef"));
        }

        void test_return_struct()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::return_struct, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("abcdef"));
        }

        void test_call_struct_on_heap()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::call_struct_on_heap, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("abcd"));
        }

        void test_for_loop()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::for_loop, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("\n"));
        }

        void test_empty_struct_overloads()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::empty_struct_overloads, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("DE"));
        }

        void test_delegate()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::delegate_str, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("fg"));
        }

        void test_statement()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::statement, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("std_out ( __STATEMENT__ ) ;string s = __STATEMENT__ ;"));
        }

        void test_real__file__()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            {
                auto guard = bstyle::parse_wrapper::with_current_file("real__file__.txt");
                utt_assert.is_true(parse(io, bstyle_test_data::real__file__, sim));
            }
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("real__file__.txt"));
        }

        void test_predefined_def()
        {
            primitive::simulator sim;
            utt_assert.is_true(bstyle::with_default_functions().compile(bstyle_test_data::predefined_def, sim));
            assert_execute_without_errors(sim);
        }

        void test_func_name_with_dot()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, bstyle_test_data::func_name_with_dot, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output().length(), static_cast<size_t>(2));
            utt_assert.equal(static_cast<int>(io.output()[0]), 1);
            utt_assert.equal(static_cast<int>(io.output()[1]), 2);
        }

        void test_errors_dot_is_disallowed_as_the_end_of_name()
        {
            primitive::simulator sim;
            bool ok = bstyle::with_default_functions().compile(bstyle_test_data::errors_dot_is_disallowed_as_the_end_of_name, sim);
            utt_assert.is_false(ok);
        }

        bool run() override
        {
            test_nlp_parsable();
            test_case1();
            test_case2();
            test_global_variable();
            test_overload_function();
            test_single_level_struct();
            test_nested_struct();
            test_function_struct();
            test_return_struct();
            test_call_struct_on_heap();
            test_for_loop();
            test_empty_struct_overloads();
            test_delegate();
            test_statement();
            test_real__file__();
            test_predefined_def();
            test_func_name_with_dot();
            test_errors_dot_is_disallowed_as_the_end_of_name();
            return true;
        }

        DEFINE_CASE(bstyle_test);
    };

    REGISTER_CASE(bstyle_test);
}
