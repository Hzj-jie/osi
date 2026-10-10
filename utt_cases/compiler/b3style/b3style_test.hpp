#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include "b3style_test_data.hpp"
#include "../b2style/b2style_test.hpp"
#include "../bstyle/bstyle_test_data.hpp"
#include "../../../compiler/b3style/b3style.hpp"

namespace osi::compiler::b3style_compiler
{
    class b3style_test : public b2style_compiler::b2style_test_runner
    {
    protected:
        bool parse(primitive::console_io::test_wrapper& io,
                   const std::string& content,
                   primitive::simulator& sim) override
        {
            return b3style::with_functions(primitive::interrupts(io.io())).compile(content, sim);
        }

        bool parse_error_case(const std::string& content) override
        {
            primitive::simulator sim;
            return b3style::with_default_functions().compile(content, sim);
        }

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

        static std::filesystem::path find_runnable_dir()
        {
            namespace fs = std::filesystem;
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
            utt_assert.is_true(parse(io, b3style_test_data::func_kw, sim));
            assert_execute_without_errors(sim);
            std::string expected = "::type0 main([])\n"
                                   "::type0 ::N::print([])\n"
                                   "::type0 ::N::f2:::Integer:::String([x: ::Integer, s: ::String])";
            utt_assert.equal(trim(io.output()), expected);
        }

        void test_b2style_func_kw()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::func_kw, sim));
            assert_execute_without_errors(sim);
            std::string expected =
                "::type0 main([])\n"
                "::type0 ::std_out:::C__struct__type__id__type([this.C__struct__type__id: ::C__struct__type__id__type&])\n"
                "::type0 ::N::print([])\n"
                "::type0 ::N::f2:::Integer:::String([x: ::Integer, s: ::String])";
            utt_assert.equal(trim(io.output()), expected);
        }

        void test_destruction_in_declaration()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b3style_test_data::destruction_in_declaration, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("constructfinishdestruct"));
        }

        void test_destruction_in_definition()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b3style_test_data::destruction_in_definition, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("createconstructfinishdestruct"));
        }

        void test_runnable()
        {
            namespace fs = std::filesystem;
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
                utt_assert.is_true(ifs.is_open());
                std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                primitive::console_io::test_wrapper io;
                primitive::simulator sim;
                auto guard = b3style::parse_wrapper::with_current_file(name);
                utt_assert.is_true(parse(io, content, sim));
                assert_execute_without_errors(sim);
            }
        }

        void test_compile_only()
        {
            logic_writer w1;
            utt_assert.is_true(b3style::with_default_functions().build(b2style_test_data::template_template_case1, w1));

            logic_writer w2;
            utt_assert.is_true(b3style::with_default_functions().build(b2style_test_data::two_template_type_parameters, w2));
        }

        void test_compile_error()
        {
            run_common_compile_errors();
            run_error(b3style_test_data::errors_define_class_constructor_for_non_class,
                      {b3style::class_initializer::failed_to_build_constructor_message("int", "x")});
            run_error(b2style_test_data::errors_cycle_typedef,
                      {"::CYCLE_TYPEDEF::A", "typedef C A"});
            run_error(b2style_test_data::errors_reinterpret_cast_without_type_id,
                      {"s.S2__struct__type__id"});
            run_error(b2style_test_data::reinterpret_cast_to_a_different_class_type,
                      {
                          b3style::class_initializer::failed_to_build_destructor_message("s")
                      });
        }

        void test_order_of_operators()
        {
            primitive::console_io::test_wrapper io;
            primitive::simulator sim;
            utt_assert.is_true(parse(io, b2style_test_data::order_of_operators, sim));
            assert_execute_without_errors(sim);
            utt_assert.equal(io.output(), std::string("15"));
        }

        void test_unused_functions_should_be_removed()
        {
            std::string logic_str;
            utt_assert.is_true(b3style::parse(b2style_test_data::case1, logic_str));
            utt_assert.is_true(logic_str.find("b2style__ufloat") == std::string::npos);
        }

        void test_template_template()
        {
            // Case 1
            {
                std::shared_ptr<osi::automata::typed_node> n;
                utt_assert.is_true(b3style_nlp()->parse(b2style_test_data::template_template_case1, n));
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
                bool ok = b3style::with_default_functions().compile(b2style_test_data::errors_duplicate_template_type_parameters, sim);
                utt_assert.is_false(ok);
            }
            // Two template parameters
            {
                std::shared_ptr<osi::automata::typed_node> n;
                utt_assert.is_true(b3style_nlp()->parse(b2style_test_data::two_template_type_parameters, n));
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

        void test_local_type_scoping()
        {
            // 1. Sibling local struct name reuse without collision
            {
                primitive::console_io::test_wrapper io;
                primitive::simulator sim;
                utt_assert.is_true(parse(io, b3style_test_data::local_struct_sibling_reuse, sim));
                assert_execute_without_errors(sim);
                std::string out = trim(io.output());
                utt_assert.is_true(out.find("Success:") != std::string::npos);
                utt_assert.is_true(out.find("Total assertions: 2") != std::string::npos);
            }

            // 2. Block-level typedef shadowing and scope unwinding
            {
                primitive::console_io::test_wrapper io;
                primitive::simulator sim;
                utt_assert.is_true(parse(io, b3style_test_data::block_typedef_shadowing, sim));
                assert_execute_without_errors(sim);
                std::string out = trim(io.output());
                utt_assert.is_true(out.find("Success:") != std::string::npos);
                utt_assert.is_true(out.find("Total assertions: 2") != std::string::npos);
            }

            // 3. Local struct encapsulation: referencing LocalSecret outside its declaring function fails to compile
            {
                run_error(b3style_test_data::errors_local_struct_encapsulation, {"LocalSecret"});
            }
        }

    public:
        bool run() override
        {
            test_func_kw();
            test_b2style_func_kw();
            test_destruction_in_declaration();
            test_destruction_in_definition();
            test_runnable();
            test_compile_only();
            test_compile_error();
            run_b2style_cases();
            test_order_of_operators();
            test_unused_functions_should_be_removed();
            test_template_template();
            test_local_type_scoping();
            return true;
        }

        DEFINE_CASE(b3style_test);
    };

    REGISTER_CASE(b3style_test);
}
