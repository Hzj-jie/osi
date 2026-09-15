#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include "b3style_test_data.hpp"
#include "../b2style/b2style_test_data.hpp"
#include "../bstyle/bstyle_test_data.hpp"
#include "../../../compiler/b3style/b3style.hpp"
#include "../../../compiler/b2style/b2style.hpp"
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"
#include "../../../interpreter/primitive/console_io.hpp"
#include "../../../automata/nlp.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace fs = std::filesystem;

namespace osi::compiler::b3style_compiler
{
    class b3style_test : public icase
    {
    private:
        static std::string trim(const std::string& s)
        {
            size_t start = 0;
            while (start < s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n'))
            {
                start++;
            }
            size_t end = s.size();
            while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n'))
            {
                end--;
            }
            return s.substr(start, end - start);
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

        static fs::path find_runnable_dir()
        {
            try
            {
                auto exe_path = fs::canonical("/proc/self/exe").parent_path();
                if (fs::exists(exe_path / "runnable"))
                {
                    return exe_path / "runnable";
                }
                if (fs::exists(exe_path / "utt_cases/compiler/b3style/runnable"))
                {
                    return exe_path / "utt_cases/compiler/b3style/runnable";
                }
                if (fs::exists(exe_path / "../utt_cases/compiler/b3style/runnable"))
                {
                    return exe_path / "../utt_cases/compiler/b3style/runnable";
                }
            }
            catch (...) {}
            if (fs::exists("utt_cases/compiler/b3style/runnable"))
            {
                return fs::canonical("utt_cases/compiler/b3style/runnable");
            }
            if (fs::exists("../utt_cases/compiler/b3style/runnable"))
            {
                return fs::canonical("../utt_cases/compiler/b3style/runnable");
            }
            if (fs::exists("tests/compiler/b3style/runnable"))
            {
                return fs::canonical("tests/compiler/b3style/runnable");
            }
            if (fs::exists("runnable"))
            {
                return fs::canonical("runnable");
            }
            return "";
        }
void test_func_kw()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    bool ok = b3style::with_functions(primitive::interrupts(io.io())).compile(b3style_test_data::func_kw, sim);
    assert(ok);
    assert_execute_without_errors(sim);
    std::string expected = "::type0 main([])\n"
                           "::type0 ::N::print([])\n"
                           "::type0 ::N::f2:::Integer:::String([x: ::Integer, s: ::String])";
    assert(trim(io.output()) == expected);
    std::cout << "[PASS] __func__\n";
}

void test_destruction_in_declaration()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    bool ok = b3style::with_functions(primitive::interrupts(io.io())).compile(b3style_test_data::destruction_in_declaration, sim);
    assert(ok);
    assert_execute_without_errors(sim);
    assert(io.output() == "constructfinishdestruct");
    std::cout << "[PASS] destruction_in_declaration\n";
}

void test_destruction_in_definition()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    bool ok = b3style::with_functions(primitive::interrupts(io.io())).compile(b3style_test_data::destruction_in_definition, sim);
    assert(ok);
    assert_execute_without_errors(sim);
    assert(io.output() == "createconstructfinishdestruct");
    std::cout << "[PASS] destruction_in_definition\n";
}

// 2. Runnable tests
void test_runnable()
{
    fs::path dir = find_runnable_dir();
    std::vector<std::string> cases = {
        "bstyle-case1.txt",
        "empty-main.txt",
        "function-call.txt",
        "hello-world.txt",
        "include-bstyle-types.txt",
        "namespace.txt",
        "predefined-macros.txt"
    };

    for (const auto& name : cases)
    {
        fs::path p = dir / name;
        std::ifstream ifs(p);
        assert(ifs.is_open());
        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        primitive::console_io::test_wrapper io;
        primitive::simulator sim;
        auto guard = b3style::parse_wrapper::with_current_file(name);
        bool ok = b3style::with_functions(primitive::interrupts(io.io())).compile(content, sim);
        assert(ok);
        assert_execute_without_errors(sim);
        std::cout << "[PASS] runnable: " << name << "\n";
    }
}

// 3. Compile-only tests
void test_compile_only()
{
    logic_writer w1;
    bool ok1 = b3style::with_default_functions().build(b2style_test_data::template_template_case1, w1);
    assert(ok1);
    std::cout << "[PASS] compile_only: template_template_case1\n";

    logic_writer w2;
    bool ok2 = b3style::with_default_functions().build(b2style_test_data::two_template_type_parameters, w2);
    assert(ok2);
    std::cout << "[PASS] compile_only: two_template_type_parameters\n";
}

// 4. Compile-error tests
void test_compile_error()
{
    std::cout << "\nStarting compile error tests...\n";
    const std::vector<std::pair<std::string, std::string>> errors = {
        {"define_class_constructor_for_non_class", b3style_test_data::errors_define_class_constructor_for_non_class},
        {"dollar_in_number", b2style_test_data::errors_dollar_in_number},
        {"include_needs_wraps", b2style_test_data::errors_include_needs_wraps},
        {"three_pluses", b2style_test_data::errors_three_pluses},
        {"value_clause_struct_type_mismatch", b2style_test_data::errors_value_clause_struct_type_mismatch},
        {"value_clause_struct_type_mismatch2", b2style_test_data::errors_value_clause_struct_type_mismatch2},
        {"function_return_struct_type_mismatch", b2style_test_data::errors_function_return_struct_type_mismatch},
        {"function_name_ends_with_dot", b2style_test_data::errors_function_name_ends_with_dot},
        {"missing_ending_quota", b2style_test_data::errors_missing_ending_quota},
        {"undefined_value_clause", b2style_test_data::errors_undefined_value_clause},
        {"reinterpret_cast_unknown_variable", b2style_test_data::errors_reinterpret_cast_unknown_variable},
        {"reinterpret_cast_unknown_type", b2style_test_data::errors_reinterpret_cast_unknown_type},
        {"reinterpret_cast_heap_with_index", b2style_test_data::errors_reinterpret_cast_heap_with_index},
        {"template_without_type_parameter", b2style_test_data::errors_template_without_type_parameter},
        {"class_initializer_for_non_class", b2style_test_data::errors_class_initializer_for_non_class},
        {"duplicate_template_type_parameters", b2style_test_data::errors_duplicate_template_type_parameters},
        {"cycle_typedef", b2style_test_data::errors_cycle_typedef},
        {"reinterpret_cast_without_type_id", b2style_test_data::errors_reinterpret_cast_without_type_id}
    };

    for (const auto& err : errors)
    {
        primitive::simulator sim;
        bool ok = b3style::with_default_functions().compile(err.second, sim);
        assert(!ok);
        std::cout << "[PASS EXPECTED FAIL] " << err.first << "\n";
    }
}

// 5. Compatibility tests with b2style test data
void check_compatibility(const std::string& name, const std::string& code, const std::string& expected, const std::string& input = "")
{
    primitive::console_io::test_wrapper io(input);
    primitive::simulator sim;
    bool ok = b3style::with_functions(primitive::interrupts(io.io())).compile(code, sim);
    assert(ok);
    assert_execute_without_errors(sim);
    assert(io.output() == expected);
    std::cout << "[PASS] compat: " << name << "\n";
}

void test_compatibility()
{
    check_compatibility("case1", b2style_test_data::case1, "Hello World");
    check_compatibility("bool_and_bool", b2style_test_data::bool_and_bool, "TrueFalseFalseFalseTrueTrueTrueFalse");
    check_compatibility("str_unescape", b2style_test_data::str_unescape, "abc\tdef\n");
    check_compatibility("_1_to_100", b2style_test_data::_1_to_100, "5050\n");
    check_compatibility("self_add", b2style_test_data::self_add, "101101102");
    check_compatibility("biguint", b2style_test_data::biguint, "429496729642949672961");
    check_compatibility("another_1_to_100", b2style_test_data::another_1_to_100, "5050\n");
    check_compatibility("while_1_to_100", b2style_test_data::while_1_to_100, "5050\n");
    check_compatibility("shift", b2style_test_data::shift, "40040012");
    check_compatibility("ifndef", b2style_test_data::ifndef, "good");
    check_compatibility("namespaces", b2style_test_data::namespaces, "a::b::f1\na::f2\na::b::f2\na::f3\na::c::f4\n");
    check_compatibility("comments", b2style_test_data::comments, "good", "good");
    check_compatibility("typedef_", b2style_test_data::typedef_, "abc");
    check_compatibility("legacy_biguint_to_str", b2style_test_data::legacy_biguint_to_str, "4294967296429496729610");
    check_compatibility("for_loop", b2style_test_data::for_loop, "495099001485019800");
    check_compatibility("negative_int", b2style_test_data::negative_int, "-1-2-3");
    check_compatibility("order_of_operators", b2style_test_data::order_of_operators, "15");
    check_compatibility("class_", b2style_test_data::class_, "200100");
    check_compatibility("nested_class", b2style_test_data::nested_class, "100200");
    check_compatibility("class_in_namespace", b2style_test_data::class_in_namespace, "123");
    check_compatibility("class_constructor", b2style_test_data::class_constructor, "10012002");
    check_compatibility("ufloat_std_out", b2style_test_data::ufloat_std_out, "1.1");
    check_compatibility("ufloat_operators", b2style_test_data::ufloat_operators, "1.10.5");
    check_compatibility("while_0_to_1", b2style_test_data::while_0_to_1, "50.5\n");
    check_compatibility("delegate_", b2style_test_data::delegate_, "abc");
    check_compatibility("delegate2", b2style_test_data::delegate2, "31");
    check_compatibility("struct_in_namespace", b2style_test_data::struct_in_namespace, "100");
    std::string expected_100_false;
    for (int i = 0; i < 100; ++i) expected_100_false += "False";
    check_compatibility("case2", b2style_test_data::case2, expected_100_false);
    check_compatibility("loaded_method", b2style_test_data::loaded_method, expected_100_false);
    check_compatibility("include", b2style_test_data::include, "abcEOF", "abc");
    check_compatibility("include2", b2style_test_data::include2, "defEoF", "def");
    check_compatibility("heap_declaration", b2style_test_data::heap_declaration, "");
    check_compatibility("struct_function_ref", b2style_test_data::struct_function_ref, "100abc");
    check_compatibility("__i__", b2style_test_data::__i__, "3_8_19_20_21_22");
    check_compatibility("function_with_global_namespace", b2style_test_data::function_with_global_namespace, "f::f");
    check_compatibility("nested_heap_access", b2style_test_data::nested_heap_access, "234");
    check_compatibility("heap_ptr_to_int64", b2style_test_data::heap_ptr_to_int64, "0 4294967296 8589934592");
    check_compatibility("empty_struct", b2style_test_data::empty_struct, "f3ff2");
    check_compatibility("calculate_pi_bbp", b2style_test_data::calculate_pi_bbp, "3.1415926535897932384626433832795028841971693993751058209749445923");
    check_compatibility("function_ref", b2style_test_data::function_ref, "12345677");
    check_compatibility("class_inheritance", b2style_test_data::class_inheritance, "3f21f2");
    check_compatibility("lots_of_semi_colons", b2style_test_data::lots_of_semi_colons, "100");
    check_compatibility("heap", b2style_test_data::heap, "abcdefghi100");
    check_compatibility("class_function_with_namespace", b2style_test_data::class_function_with_namespace, "100200");
    check_compatibility("delegate_ref", b2style_test_data::delegate_ref, "100101");
    check_compatibility("delegate_template", b2style_test_data::delegate_template, "2abc1");
    check_compatibility("function_ptr", b2style_test_data::function_ptr, "10199");
    check_compatibility("nested_template", b2style_test_data::nested_template, "C");
    check_compatibility("primitive_template", b2style_test_data::primitive_template, "101101.11");
    check_compatibility("reinterpret_cast_", b2style_test_data::reinterpret_cast_, "-100100");
    check_compatibility("reinterpret_cast_heap", b2style_test_data::reinterpret_cast_heap, "-1010");
    check_compatibility("template_", b2style_test_data::template_, "DE");
    check_compatibility("template_with_different_length", b2style_test_data::template_with_different_length, "12abc");
    check_compatibility("template_wont_be_extended_twice", b2style_test_data::template_wont_be_extended_twice, "24");
    check_compatibility("reinterpret_cast_to_a_different_class_type", b2style_test_data::reinterpret_cast_to_a_different_class_type, "");
}

// 6. Custom verification tests ported from b2style
void test_multiline_string()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::multiline_string, sim));
    assert_execute_without_errors(sim);
    std::string out = io.output();
    std::string normalized;
    for (size_t i = 0; i < out.size(); ++i)
    {
        if (out[i] == '\r') continue;
        normalized += out[i];
    }
    assert(normalized == "a\n    b\n    c\n    d");
    std::cout << "[PASS] multiline_string\n";
}

void test_i_post()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::i__, sim));
    assert_execute_without_errors(sim);
    assert(io.output().size() >= 1);
    assert(static_cast<int>(io.output()[0]) == 1);
    std::cout << "[PASS] i__\n";
}

void test_i_post_2()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::i__2, sim));
    assert_execute_without_errors(sim);
    assert(io.output().size() >= 4);
    assert(static_cast<int>(io.output()[0]) == 1);
    assert(static_cast<int>(io.output()[1]) == 2);
    assert(static_cast<int>(io.output()[2]) == 3);
    assert(static_cast<int>(io.output()[3]) == 4);
    std::cout << "[PASS] i__2\n";
}

void test_i_pre()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::__i, sim));
    assert_execute_without_errors(sim);
    assert(io.output().size() >= 4);
    assert(static_cast<int>(io.output()[0]) == 1);
    assert(static_cast<int>(io.output()[1]) == 2);
    assert(static_cast<int>(io.output()[2]) == 3);
    assert(static_cast<int>(io.output()[3]) == 4);
    std::cout << "[PASS] __i\n";
}

void test_heap_function_ref()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::heap_function_ref, sim));
    assert_execute_without_errors(sim);
    std::string expected;
    for (int i = 1; i <= 100; ++i) expected += std::to_string(i) + "\n";
    assert(io.output() == expected);
    std::cout << "[PASS] heap_function_ref\n";
}

void test_nested_paragraph()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::nested_paragraph, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "hello world\nhello again world\nhello again again world");
    std::cout << "[PASS] nested_paragraph\n";
}

void test_test_assert()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::test_assert, sim));
    assert_execute_without_errors(sim);
    std::string out = io.output();
    assert(out.find("Failure: no extra information.") != std::string::npos);
    assert(out.find("Success: no extra information.") != std::string::npos);
    assert(out.find("Total assertions: 2") != std::string::npos);
    std::cout << "[PASS] test_assert\n";
}

void test_assert_()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::assert_, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "This assertion should not pass.");
    std::cout << "[PASS] assert_\n";
}

void test_assert_with_statement()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::assert_with_statement, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "assert ( __STATEMENT__ , i < 100 , \"line 8\" ) ;: line 8");
    std::cout << "[PASS] assert_with_statement\n";
}

void test_class_on_heap()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::class_on_heap, sim));
    assert_execute_without_errors(sim);
    std::string expected;
    for (int i = 0; i < 100; ++i)
    {
        expected += std::to_string(i) + " " + std::to_string(i) + " " + std::to_string(i) + "\n";
    }
    assert(io.output() == expected);
    std::cout << "[PASS] class_on_heap\n";
}

void test_vector_destructor()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(b3style::with_functions(primitive::interrupts(io.io())).compile(b2style_test_data::vector_destructor, sim));
    assert_execute_without_errors(sim);
    assert(sim.mem().get_heap_array(1) == nullptr);
    std::cout << "[PASS] vector_destructor\n";
}

void test_unused_functions_should_be_removed()
{
    std::string logic_str;
    assert(b3style::parse(b2style_test_data::case1, logic_str));
    assert(logic_str.find("b2style__ufloat") == std::string::npos);
    std::cout << "[PASS] unused_functions_should_be_removed\n";
}

void test_template_template()
{
    std::cout << "\nStarting template_template tests...\n";
    // Case 1
    {
        std::shared_ptr<osi::automata::typed_node> n;
        assert(b3style_nlp()->parse(b2style_test_data::template_template_case1, n));
        assert(n != nullptr);
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
        assert(tmpl->apply({"int"}, impl));
        assert(impl == "class C__int { int x ; void f ( int y ) { } } ;");
        std::cout << "[PASS] template_template_case1\n";
    }
    // Disallow duplicated template type parameters
    {
        primitive::simulator sim;
        bool ok = b3style::with_default_functions().compile(b2style_test_data::errors_duplicate_template_type_parameters, sim);
        assert(!ok);
        std::cout << "[PASS] template_template_disallow_duplicated_params\n";
    }
    // Two template parameters
    {
        std::shared_ptr<osi::automata::typed_node> n;
        assert(b3style_nlp()->parse(b2style_test_data::two_template_type_parameters, n));
        assert(n != nullptr);
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
        assert(tmpl->apply({"int", "string"}, impl));
        assert(impl == "class C__int__string { int x ; string y ; void p ( int x , string y ) { } } ;");
        std::cout << "[PASS] template_template_two_params\n";
    }
}


    public:
        bool run() override
        {
            test_func_kw();
            test_destruction_in_declaration();
            test_destruction_in_definition();
            test_runnable();
            test_compile_only();
            test_compile_error();
            test_compatibility();
            test_multiline_string();
            test_i_post();
            test_i_post_2();
            test_i_pre();
            test_heap_function_ref();
            test_nested_paragraph();
            test_test_assert();
            test_assert_();
            test_assert_with_statement();
            test_class_on_heap();
            test_vector_destructor();
            test_unused_functions_should_be_removed();
            test_template_template();
            return true;
        }

        DEFINE_CASE(b3style_test);
    };

    REGISTER_CASE(b3style_test);
}
