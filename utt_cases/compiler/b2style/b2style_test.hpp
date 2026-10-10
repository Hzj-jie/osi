#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include "b2style_test_data.hpp"
#include "../compile_error_test_runner.hpp"
#include "../../../compiler/b2style/b2style.hpp"
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"
#include "../../../interpreter/primitive/console_io.hpp"
#include "../../../automata/nlp.hpp"
#include "../../../automata/syntax/syntaxer.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace osi::compiler::b2style_compiler
{
    class b2style_compile_error_test_runner : public compile_error_test_runner
    {
    protected:
        void run_common_compile_errors()
        {
            run_error(b2style_test_data::errors_dollar_in_number, {"nlexer", "$"});
            run_error(b2style_test_data::errors_include_needs_wraps,
                      {
                          automata::syntaxer::debug_str("abc", "raw-name"),
                          automata::syntaxer::debug_str(".", "dot"),
                          automata::syntaxer::debug_str("h", "raw-name")
                      });
            run_error(b2style_test_data::errors_three_pluses,
                      {
                          automata::syntaxer::debug_str("void", "raw-name"),
                          automata::syntaxer::debug_str("main", "raw-name"),
                          automata::syntaxer::debug_str("i", "raw-name"),
                          automata::syntaxer::debug_str("++", "self-inc"),
                          automata::syntaxer::debug_str("+", "add")
                      });
            run_error(b2style_test_data::errors_value_clause_struct_type_mismatch, {"S2", "s", "S1"});
            run_error(b2style_test_data::errors_value_clause_struct_type_mismatch2, {"S2", "s", "S1"});
            run_error(b2style_test_data::errors_function_return_struct_type_mismatch, {"S2", "f", "S1"});
            run_error(b2style_test_data::errors_function_name_ends_with_dot,
                      {
                          automata::syntaxer::debug_str("c", "raw-name"),
                          automata::syntaxer::debug_str(".", "dot")
                      });
            run_error(b2style_test_data::errors_missing_ending_quota, {"[nlexer]", "\""});
            run_error(b2style_test_data::errors_undefined_value_clause, {"this_is_an_undefined_value_clause"});
            run_error(b2style_test_data::errors_reinterpret_cast_unknown_variable, {"this_is_an_unknown_variable"});
            run_error(b2style_test_data::errors_reinterpret_cast_unknown_type, {"this_is_an_unknown_type"});
            run_error(b2style_test_data::errors_reinterpret_cast_heap_with_index, {"s[0]"});
            run_error(b2style_test_data::errors_template_without_type_parameter, {"ThisTemplateHasNoTypeParameter"});
            run_error(b2style_test_data::errors_class_initializer_for_non_class, {"ooops_this_is_not_a_class"});
            run_error(b2style_test_data::errors_duplicate_template_type_parameters, {"T, T"});
        }
    };

    class b2style_test_runner : public b2style_compile_error_test_runner
    {
    protected:
        virtual bool parse(primitive::console_io::test_wrapper& io,
                           const std::string& content,
                           primitive::simulator& sim) = 0;

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
        void test_case1()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::case1, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("Hello World"));
        }

        void test_case2()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::case2, sim));
            assert_execute_without_errors(sim);
            std::string expected;
            for (int i = 0; i < 100; ++i) expected += "False";
            utt_assert.equal(io.output(), expected);
        }

        void test_bool_and_bool()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::bool_and_bool, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("TrueFalseFalseFalseTrueTrueTrueFalse"));
        }

        void test_str_unescape()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::str_unescape, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("abc\tdef\n"));
        }

        void test_1_to_100()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::_1_to_100, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("5050\n"));
        }

        void test_self_add()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::self_add, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("101101102"));
        }

        void test_biguint()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::biguint, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("429496729642949672961"));
        }

        void test_another_1_to_100()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::another_1_to_100, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("5050\n"));
        }

        void test_loaded_method()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::loaded_method, sim));
            assert_execute_without_errors(sim);
            std::string expected;
            for (int i = 0; i < 100; ++i) expected += "False";
            utt_assert.equal(io.output(), expected);
        }

        void test_while_1_to_100()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::while_1_to_100, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("5050\n"));
        }

        void test_shift()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::shift, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("40040012"));
        }

        void test_include()
        {
            primitive::console_io::test_wrapper io("abc");
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::include, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("abcEOF"));
        }

        void test_include2()
        {
            primitive::console_io::test_wrapper io("def");
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::include2, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("defEoF"));
        }

        void test_ifndef()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::ifndef, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("good"));
        }

        void test_namespaces()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::namespaces, sim));
            assert_execute_without_errors(sim);
            std::string expected = "a::b::f1\na::f2\na::b::f2\na::f3\na::c::f4\n";
            utt_assert.equal(io.output(), expected);
        }

        void test_multiline_string()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::multiline_string, sim));
            assert_execute_without_errors(sim);
            std::string out = io.output();
            std::string normalized;
            for (size_t i = 0; i < out.size(); ++i)
            {
                if (out[i] == '\r') continue;
                normalized += out[i];
            }
            utt_assert.equal(normalized, std::string("a\n    b\n    c\n    d"));
        }

        void test_comments()
        {
            primitive::console_io::test_wrapper io("good");
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::comments, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("good"));
        }

        void test_typedef()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::typedef_, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("abc"));
        }

        void test_legacy_biguint_to_str()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::legacy_biguint_to_str, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("4294967296429496729610"));
        }

        void test_heap_declaration()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::heap_declaration, sim));
            assert_execute_without_errors(sim);
        }

        void test_struct_function_ref()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::struct_function_ref, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("100abc"));
        }

        void test_i_pre_post()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::__i__, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("3_8_19_20_21_22"));
        }

        void test_for_loop()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::for_loop, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("495099001485019800"));
        }

        void test_i_post()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::i__, sim));
            assert_execute_without_errors(sim);
            utt_assert.is_true(io.output().size() >= 1);
            utt_assert.equal(static_cast<int>(io.output()[0]), 1);
        }

        void test_i_post_2()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::i__2, sim));
            assert_execute_without_errors(sim);
            utt_assert.is_true(io.output().size() >= 4);
            utt_assert.equal(static_cast<int>(io.output()[0]), 1);
            utt_assert.equal(static_cast<int>(io.output()[1]), 2);
            utt_assert.equal(static_cast<int>(io.output()[2]), 3);
            utt_assert.equal(static_cast<int>(io.output()[3]), 4);
        }

        void test_i_pre()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::__i, sim));
            assert_execute_without_errors(sim);
            utt_assert.is_true(io.output().size() >= 4);
            utt_assert.equal(static_cast<int>(io.output()[0]), 1);
            utt_assert.equal(static_cast<int>(io.output()[1]), 2);
            utt_assert.equal(static_cast<int>(io.output()[2]), 3);
            utt_assert.equal(static_cast<int>(io.output()[3]), 4);
        }

        void test_heap_function_ref()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::heap_function_ref, sim));
            assert_execute_without_errors(sim);
            std::string expected;
            for (int i = 1; i <= 100; ++i) expected += std::to_string(i) + "\n";
            utt_assert.equal(io.output(), expected);
        }

        void test_nested_paragraph()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::nested_paragraph, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("hello world\nhello again world\nhello again again world"));
        }

        void test_function_with_global_namespace()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::function_with_global_namespace, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("f::f"));
        }

        void test_nested_heap_access()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::nested_heap_access, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("234"));
        }

        void test_heap_ptr_to_int64()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::heap_ptr_to_int64, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("0 4294967296 8589934592"));
        }

        void test_empty_struct()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::empty_struct, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("f3ff2"));
        }

        void test_delegate()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::delegate_, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("abc"));
        }

        void test_delegate2()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::delegate2, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("31"));
        }

        void test_struct_in_namespace()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::struct_in_namespace, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("100"));
        }

        void test_class()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::class_, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("200100"));
        }

        void test_nested_class()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::nested_class, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("100200"));
        }

        void test_class_in_namespace()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::class_in_namespace, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("123"));
        }

        void test_class_constructor()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::class_constructor, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("10012002"));
        }

        void test_negative_int()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::negative_int, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("-1-2-3"));
        }

        void test_ufloat_std_out()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::ufloat_std_out, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("1.1"));
        }

        void test_ufloat_operators()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::ufloat_operators, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("1.10.5"));
        }

        void test_while_0_to_1()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::while_0_to_1, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("50.5\n"));
        }

        void test_calculate_pi_bbp()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::calculate_pi_bbp, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("3.1415926535897932384626433832795028841971693993751058209749445923"));
        }

        void test_function_ref()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::function_ref, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("12345677"));
        }

        void test_class_inheritance()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::class_inheritance, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("3f21f2"));
        }

        void test_lots_of_semi_colons()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::lots_of_semi_colons, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("100"));
        }

        void test_test_assert()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::test_assert, sim));
            assert_execute_without_errors(sim);
            std::string out = io.output();
            utt_assert.is_true(out.find("Failure: no extra information.") != std::string::npos);
            utt_assert.is_true(out.find("Success: no extra information.") != std::string::npos);
            utt_assert.is_true(out.find("Total assertions: 2") != std::string::npos);
        }

        void test_assert_()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::assert_, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("This assertion should not pass."));
        }

        void test_assert_with_statement()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::assert_with_statement, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("assert ( __STATEMENT__ , i < 100 , \"line 8\" ) ;: line 8"));
        }

        void test_heap()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::heap, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("abcdefghi100"));
        }

        void test_class_on_heap()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::class_on_heap, sim));
            assert_execute_without_errors(sim);
            std::string expected;
            for (int i = 0; i < 100; ++i)
            {
                expected += std::to_string(i) + " " + std::to_string(i) + " " + std::to_string(i) + "\n";
            }
            utt_assert.equal(io.output(), expected);
        }

        void test_class_function_with_namespace()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::class_function_with_namespace, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("100200"));
        }

        void test_delegate_ref()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::delegate_ref, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("100101"));
        }

        void test_delegate_template()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::delegate_template, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("2abc1"));
        }

        void test_function_ptr()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::function_ptr, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("10199"));
        }

        void test_nested_template()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::nested_template, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("C"));
        }

        void test_primitive_template()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::primitive_template, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("101101.11"));
        }

        void test_reinterpret_cast()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::reinterpret_cast_, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("-100100"));
        }

        void test_reinterpret_cast_heap()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::reinterpret_cast_heap, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("-1010"));
        }

        void test_template()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::template_, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("DE"));
        }

        void test_template_with_different_length()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::template_with_different_length, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("12abc"));
        }

        void test_template_wont_be_extended_twice()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::template_wont_be_extended_twice, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("24"));
        }

        void test_vector_destructor()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::vector_destructor, sim));
            assert_execute_without_errors(sim);
            utt_assert.is_true(sim.mem().get_heap_array(1) == nullptr);
        }

        void run_b2style_cases()
        {
            test_case1();
            test_case2();
            test_bool_and_bool();
            test_str_unescape();
            test_1_to_100();
            test_self_add();
            test_biguint();
            test_another_1_to_100();
            test_loaded_method();
            test_while_1_to_100();
            test_shift();
            test_include();
            test_include2();
            test_ifndef();
            test_namespaces();
            test_multiline_string();
            test_comments();
            test_typedef();
            test_legacy_biguint_to_str();
            test_heap_declaration();
            test_struct_function_ref();
            test_i_pre_post();
            test_for_loop();
            test_i_post();
            test_i_post_2();
            test_i_pre();
            test_heap_function_ref();
            test_nested_paragraph();
            test_function_with_global_namespace();
            test_nested_heap_access();
            test_heap_ptr_to_int64();
            test_empty_struct();
            test_delegate();
            test_delegate2();
            test_struct_in_namespace();
            test_class();
            test_nested_class();
            test_class_in_namespace();
            test_class_constructor();
            test_negative_int();
            test_ufloat_std_out();
            test_ufloat_operators();
            test_while_0_to_1();
            test_calculate_pi_bbp();
            test_function_ref();
            test_class_inheritance();
            test_lots_of_semi_colons();
            test_test_assert();
            test_assert_();
            test_assert_with_statement();
            test_heap();
            test_class_on_heap();
            test_class_function_with_namespace();
            test_delegate_ref();
            test_delegate_template();
            test_function_ptr();
            test_nested_template();
            test_primitive_template();
            test_reinterpret_cast();
            test_reinterpret_cast_heap();
            test_template();
            test_template_with_different_length();
            test_template_wont_be_extended_twice();
            test_vector_destructor();
        }
    };

    class b2style_test : public b2style_test_runner
    {
    protected:
        bool parse(primitive::console_io::test_wrapper& io,
                   const std::string& content,
                   primitive::simulator& sim) override
        {
            return b2style::with_functions(primitive::interrupts(io.io())).compile(content, sim);
        }

        bool parse_error_case(const std::string& content) override
        {
            primitive::simulator sim;
            return b2style::with_default_functions().compile(content, sim);
        }

    public:
        void test_nlp_parsable()
        {
            std::shared_ptr<osi::automata::nlp> parser;
            bool ok = osi::automata::nlp::of_file(b2style::nlexer_rule(),
                                                  b2style::syntaxer_rule(),
                                                  parser);
            utt_assert.is_true(ok);
            utt_assert.is_true(parser != nullptr);
        }

        void test_unused_functions_should_be_removed()
        {
            std::string bstyle_str;
            utt_assert.is_true(b2style::parse(b2style_test_data::case1, bstyle_str));
            utt_assert.is_true(bstyle_str.find("b2style__ufloat__from") == std::string::npos);
            std::string logic_str;
            utt_assert.is_true(bstyle_compiler::bstyle::parse(bstyle_str, logic_str));
            utt_assert.is_true(logic_str.find("b2style__ufloat") == std::string::npos);
        }

        void test_order_of_operators()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::order_of_operators, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("15"));
        }

        void test_reinterpret_cast_to_a_different_class_type()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::reinterpret_cast_to_a_different_class_type, sim));
            assert_execute_without_errors(sim);
        }

        void test_func()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::func_kw, sim));
            assert_execute_without_errors(sim);
            std::string out = io.output();
            while (!out.empty() && (out.back() == '\r' || out.back() == '\n' || out.back() == ' '))
            {
                out.pop_back();
            }
            std::string expected =
                "type0 main([])\n"
                "type0 std_out:C__struct__type__id__type([this.C__struct__type__id: C__struct__type__id__type&])\n"
                "type0 N__print([])\n"
                "type0 N__f2:Integer:String([N__x: Integer, N__s: String])";
            utt_assert.equal(out, expected);
        }

        void test_compile_errors()
        {
            run_common_compile_errors();
            run_error(b2style_test_data::errors_cycle_typedef, {"CYCLE_TYPEDEF__A", "CYCLE_TYPEDEF__C"});
            run_error(b2style_test_data::errors_reinterpret_cast_without_type_id, {"s.S2__struct__type__id"});
        }

        void test_template_template()
        {
            // Case 1
            {
                std::shared_ptr<osi::automata::typed_node> n;
                utt_assert.is_true(b2style_nlp()->parse(b2style_test_data::template_template_case1, n));
                utt_assert.is_true(n != nullptr);
                auto tmpl_node = n->child(0)->child(0);
                auto head = tmpl_node->child(0);
                auto body = tmpl_node->child(1)->child(0);
                auto type_param_list = head->child(2);
                std::vector<std::string> type_params;
                for (uint32_t i = 0; i < type_param_list->child_count(); ++i)
                {
                    auto p = type_param_list->child(i);
                    if (p->type_name == "type-param-with-comma") p = p->child(0);
                    type_params.push_back(p->input_without_ignored());
                }
                auto name_node = body->child(1);
                auto tmpl = std::make_shared<template_template>(body, name_node, type_params);
                std::string impl;
                utt_assert.is_true(tmpl->apply({"int"}, impl));
                utt_assert.equal(impl, std::string("class C__int { int x ; void f ( int y ) { } } ;"));
            }
            // Disallow duplicated template type parameters
            {
                primitive::simulator sim;
                bool ok = b2style::with_default_functions().compile(b2style_test_data::errors_duplicate_template_type_parameters, sim);
                utt_assert.is_false(ok);
            }
            // Two template parameters
            {
                std::shared_ptr<osi::automata::typed_node> n;
                utt_assert.is_true(b2style_nlp()->parse(b2style_test_data::two_template_type_parameters, n));
                utt_assert.is_true(n != nullptr);
                auto tmpl_node = n->child(0)->child(0);
                auto head = tmpl_node->child(0);
                auto body = tmpl_node->child(1)->child(0);
                auto type_param_list = head->child(2);
                std::vector<std::string> type_params;
                for (uint32_t i = 0; i < type_param_list->child_count(); ++i)
                {
                    auto p = type_param_list->child(i);
                    if (p->type_name == "type-param-with-comma") p = p->child(0);
                    type_params.push_back(p->input_without_ignored());
                }
                auto name_node = body->child(1);
                auto tmpl = std::make_shared<template_template>(body, name_node, type_params);
                std::string impl;
                utt_assert.is_true(tmpl->apply({"int", "string"}, impl));
                utt_assert.equal(impl, std::string("class C__int__string { int x ; string y ; void p ( int x , string y ) { } } ;"));
            }
        }

        bool run() override
        {
            test_nlp_parsable();
            run_b2style_cases();
            test_unused_functions_should_be_removed();
            test_order_of_operators();
            test_reinterpret_cast_to_a_different_class_type();
            test_func();
            test_compile_errors();
            test_template_template();
            return true;
        }

        DEFINE_CASE(b2style_test);
    };

    REGISTER_CASE(b2style_test);
}
