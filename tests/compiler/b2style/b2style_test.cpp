#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include <algorithm>
#include "b2style_test_data.hpp"
#include "../../../compiler/b2style/b2style.hpp"
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"
#include "../../../interpreter/primitive/console_io.hpp"
#include "../../../automata/nlp.hpp"

using namespace osi::compiler;
using namespace osi::compiler::b2style_compiler;

static bool parse(primitive::console_io::test_wrapper& io,
                  const std::string& content,
                  primitive::simulator& sim)
{
    return b2style::with_functions(primitive::interrupts(io.io())).compile(content, sim);
}

static void assert_execute_without_errors(primitive::simulator& sim)
{
    sim.execute();
    if (sim.halt())
    {
        std::cerr << "Execution halted with error: " << sim.halt_error() << std::endl;
    }
    assert(!sim.halt());
}

void test_case1()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::case1, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "Hello World");
    std::cout << "[PASS] case1\n";
}

void test_case2()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::case2, sim));
    assert_execute_without_errors(sim);
    std::string expected;
    for (int i = 0; i < 100; ++i) expected += "False";
    assert(io.output() == expected);
    std::cout << "[PASS] case2\n";
}

void test_bool_and_bool()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::bool_and_bool, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "TrueFalseFalseFalseTrueTrueTrueFalse");
    std::cout << "[PASS] bool_and_bool\n";
}

void test_str_unescape()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::str_unescape, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "abc\tdef\n");
    std::cout << "[PASS] str_unescape\n";
}

void test_1_to_100()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::_1_to_100, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "5050\n");
    std::cout << "[PASS] _1_to_100\n";
}

void test_self_add()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::self_add, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "101101102");
    std::cout << "[PASS] self_add\n";
}

void test_biguint()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::biguint, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "429496729642949672961");
    std::cout << "[PASS] biguint\n";
}

void test_another_1_to_100()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::another_1_to_100, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "5050\n");
    std::cout << "[PASS] another_1_to_100\n";
}

void test_loaded_method()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::loaded_method, sim));
    assert_execute_without_errors(sim);
    std::string expected;
    for (int i = 0; i < 100; ++i) expected += "False";
    assert(io.output() == expected);
    std::cout << "[PASS] loaded_method\n";
}

void test_while_1_to_100()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::while_1_to_100, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "5050\n");
    std::cout << "[PASS] while_1_to_100\n";
}

void test_pi_integral_0_1()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::pi_integral_0_1, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "3.304518326248318338508394330371205830216509246678390049350471115");
    std::cout << "[PASS] pi_integral_0_1\n";
}

void test_shift()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::shift, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "40040012");
    std::cout << "[PASS] shift\n";
}

void test_include()
{
    primitive::console_io::test_wrapper io("abc");
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::include, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "abcEOF");
    std::cout << "[PASS] include\n";
}

void test_include2()
{
    primitive::console_io::test_wrapper io("def");
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::include2, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "defEoF");
    std::cout << "[PASS] include2\n";
}

void test_ifndef()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::ifndef, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "good");
    std::cout << "[PASS] ifndef\n";
}

void test_namespaces()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::namespaces, sim));
    assert_execute_without_errors(sim);
    std::string expected = "a::b::f1\na::f2\na::b::f2\na::f3\na::c::f4\n";
    assert(io.output() == expected);
    std::cout << "[PASS] namespaces\n";
}

void test_multiline_string()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::multiline_string, sim));
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

void test_comments()
{
    primitive::console_io::test_wrapper io("good");
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::comments, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "good");
    std::cout << "[PASS] comments\n";
}

void test_typedef()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::typedef_, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "abc");
    std::cout << "[PASS] typedef\n";
}

void test_legacy_biguint_to_str()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::legacy_biguint_to_str, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "4294967296429496729610");
    std::cout << "[PASS] legacy_biguint_to_str\n";
}

void test_heap_declaration()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::heap_declaration, sim));
    assert_execute_without_errors(sim);
    std::cout << "[PASS] heap_declaration\n";
}

void test_struct_function_ref()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::struct_function_ref, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "100abc");
    std::cout << "[PASS] struct_function_ref\n";
}

void test_i_pre_post()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::__i__, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "3_8_19_20_21_22");
    std::cout << "[PASS] __i__\n";
}

void test_for_loop()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::for_loop, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "495099001485019800");
    std::cout << "[PASS] for_loop\n";
}

void test_i_post()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::i__, sim));
    assert_execute_without_errors(sim);
    assert(io.output().size() >= 1);
    assert(static_cast<int>(io.output()[0]) == 1);
    std::cout << "[PASS] i__\n";
}

void test_i_post_2()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::i__2, sim));
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
    assert(parse(io, b2style_test_data::__i, sim));
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
    assert(parse(io, b2style_test_data::heap_function_ref, sim));
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
    assert(parse(io, b2style_test_data::nested_paragraph, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "hello world\nhello again world\nhello again again world");
    std::cout << "[PASS] nested_paragraph\n";
}

void test_function_with_global_namespace()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::function_with_global_namespace, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "f::f");
    std::cout << "[PASS] function_with_global_namespace\n";
}

void test_nested_heap_access()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::nested_heap_access, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "234");
    std::cout << "[PASS] nested_heap_access\n";
}

void test_heap_ptr_to_int64()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::heap_ptr_to_int64, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "0 4294967296 8589934592");
    std::cout << "[PASS] heap_ptr_to_int64\n";
}

void test_unused_functions_should_be_removed()
{
    std::string bstyle_str;
    assert(b2style::parse(b2style_test_data::case1, bstyle_str));
    assert(bstyle_str.find("b2style__ufloat__from") == std::string::npos);
    std::string logic_str;
    assert(bstyle_compiler::bstyle::parse(bstyle_str, logic_str));
    assert(logic_str.find("b2style__ufloat") == std::string::npos);
    std::cout << "[PASS] unused_functions_should_be_removed\n";
}

void test_empty_struct()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::empty_struct, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "f3ff2");
    std::cout << "[PASS] empty_struct\n";
}

void test_delegate()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::delegate_, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "abc");
    std::cout << "[PASS] delegate\n";
}

void test_delegate2()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::delegate2, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "31");
    std::cout << "[PASS] delegate2\n";
}

void test_struct_in_namespace()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::struct_in_namespace, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "100");
    std::cout << "[PASS] struct_in_namespace\n";
}

void test_class()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::class_, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "200100");
    std::cout << "[PASS] class\n";
}

void test_nested_class()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::nested_class, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "100200");
    std::cout << "[PASS] nested_class\n";
}

void test_class_in_namespace()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::class_in_namespace, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "123");
    std::cout << "[PASS] class_in_namespace\n";
}

void test_class_constructor()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::class_constructor, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "10012002");
    std::cout << "[PASS] class_constructor\n";
}

void test_negative_int()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::negative_int, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "-1-2-3");
    std::cout << "[PASS] negative_int\n";
}

void test_ufloat_std_out()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::ufloat_std_out, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "1.1");
    std::cout << "[PASS] ufloat_std_out\n";
}

void test_ufloat_operators()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::ufloat_operators, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "1.10.5");
    std::cout << "[PASS] ufloat_operators\n";
}

void test_while_0_to_1()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::while_0_to_1, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "50.5\n");
    std::cout << "[PASS] while_0_to_1\n";
}

void test_calculate_pi_bbp()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::calculate_pi_bbp, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "3.1415926535897932384626433832795028841971693993751058209749445923");
    std::cout << "[PASS] calculate_pi_bbp\n";
}

void test_function_ref()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::function_ref, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "12345677");
    std::cout << "[PASS] function_ref\n";
}

void test_class_inheritance()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::class_inheritance, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "3f21f2");
    std::cout << "[PASS] class_inheritance\n";
}

void test_lots_of_semi_colons()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::lots_of_semi_colons, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "100");
    std::cout << "[PASS] lots_of_semi_colons\n";
}

void test_test_assert()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::test_assert, sim));
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
    assert(parse(io, b2style_test_data::assert_, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "This assertion should not pass.");
    std::cout << "[PASS] assert_\n";
}

void test_assert_with_statement()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::assert_with_statement, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "assert ( __STATEMENT__ , i < 100 , \"line 8\" ) ;: line 8");
    std::cout << "[PASS] assert_with_statement\n";
}

void test_heap()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::heap, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "abcdefghi100");
    std::cout << "[PASS] heap\n";
}

void test_class_on_heap()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::class_on_heap, sim));
    assert_execute_without_errors(sim);
    std::string expected;
    for (int i = 0; i < 100; ++i)
    {
        expected += std::to_string(i) + " " + std::to_string(i) + " " + std::to_string(i) + "\n";
    }
    assert(io.output() == expected);
    std::cout << "[PASS] class_on_heap\n";
}

void test_class_function_with_namespace()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::class_function_with_namespace, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "100200");
    std::cout << "[PASS] class_function_with_namespace\n";
}

void test_delegate_ref()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::delegate_ref, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "100101");
    std::cout << "[PASS] delegate_ref\n";
}

void test_delegate_template()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::delegate_template, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "2abc1");
    std::cout << "[PASS] delegate_template\n";
}

void test_function_ptr()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::function_ptr, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "10199");
    std::cout << "[PASS] function_ptr\n";
}

void test_nested_template()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::nested_template, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "C");
    std::cout << "[PASS] nested_template\n";
}

void test_primitive_template()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::primitive_template, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "101101.11");
    std::cout << "[PASS] primitive_template\n";
}

void test_reinterpret_cast()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::reinterpret_cast_, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "-100100");
    std::cout << "[PASS] reinterpret_cast\n";
}

void test_reinterpret_cast_heap()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::reinterpret_cast_heap, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "-1010");
    std::cout << "[PASS] reinterpret_cast_heap\n";
}

void test_template()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::template_, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "DE");
    std::cout << "[PASS] template\n";
}

void test_template_with_different_length()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::template_with_different_length, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "12abc");
    std::cout << "[PASS] template_with_different_length\n";
}

void test_template_wont_be_extended_twice()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::template_wont_be_extended_twice, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "24");
    std::cout << "[PASS] template_wont_be_extended_twice\n";
}

void test_vector_destructor()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::vector_destructor, sim));
    assert_execute_without_errors(sim);
    assert(sim.mem().get_heap_array(1) == nullptr);
    std::cout << "[PASS] vector_destructor\n";
}

void test_order_of_operators()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::order_of_operators, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "15");
    std::cout << "[PASS] order_of_operators\n";
}

void test_reinterpret_cast_to_a_different_class_type()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::reinterpret_cast_to_a_different_class_type, sim));
    assert_execute_without_errors(sim);
    std::cout << "[PASS] reinterpret_cast_to_a_different_class_type\n";
}

void test_func()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, b2style_test_data::func_kw, sim));
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
    assert(out == expected);
    std::cout << "[PASS] __func__\n";
}

int main()
{
    std::cout << "Running b2style tests...\n";
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
    // test_pi_integral_0_1 is <command_line_specified> in osi.net
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
    test_unused_functions_should_be_removed();
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
    test_order_of_operators();
    test_reinterpret_cast_to_a_different_class_type();
    test_func();

    std::cout << "\nALL 60 B2STYLE TESTS PASSED!\n";
    return 0;
}
