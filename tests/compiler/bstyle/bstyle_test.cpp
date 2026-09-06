#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include "bstyle_test_data.hpp"
#include "../../../compiler/bstyle/bstyle.hpp"
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"
#include "../../../interpreter/primitive/console_io.hpp"
#include "../../../automata/nlp.hpp"

using namespace osi::compiler;
using namespace osi::compiler::bstyle_compiler;

static bool parse(primitive::console_io::test_wrapper& io,
                  const std::string& content,
                  primitive::simulator& sim)
{
    return bstyle::with_functions(primitive::interrupts(io.io())).compile(content, sim);
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
    assert(parse(io, bstyle_test_data::case1, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "Hello World");
    std::cout << "[PASS] case1\n";
}

void test_case2()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::case2, sim));
    assert_execute_without_errors(sim);
    std::string expected;
    for (int i = 0; i < 100; ++i)
    {
        expected += "False";
    }
    assert(io.output() == expected);
    std::cout << "[PASS] case2\n";
}

void test_global_variable()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::global_variable, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "TrueFalse");
    std::cout << "[PASS] global_variable\n";
}

void test_overload_function()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::overload_function, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "TrueFalseFalseTrue");
    std::cout << "[PASS] overload_function\n";
}

void test_single_level_struct()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::single_level_struct, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "dabc");
    std::cout << "[PASS] single_level_struct\n";
}

void test_nested_struct()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::nested_struct, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "dd");
    std::cout << "[PASS] nested_struct\n";
}

void test_function_struct()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::function_struct, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "abcdef");
    std::cout << "[PASS] function_struct\n";
}

void test_return_struct()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::return_struct, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "abcdef");
    std::cout << "[PASS] return_struct\n";
}

void test_call_struct_on_heap()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::call_struct_on_heap, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "abcd");
    std::cout << "[PASS] call_struct_on_heap\n";
}

void test_for_loop()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::for_loop, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "\n");
    std::cout << "[PASS] for_loop\n";
}

void test_empty_struct_overloads()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::empty_struct_overloads, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "DE");
    std::cout << "[PASS] empty_struct_overloads\n";
}

void test_delegate()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::delegate_str, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "fg");
    std::cout << "[PASS] delegate\n";
}

void test_statement()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::statement, sim));
    assert_execute_without_errors(sim);
    assert(io.output() == "std_out ( __STATEMENT__ ) ;string s = __STATEMENT__ ;");
    std::cout << "[PASS] statement\n";
}

void test_nlp_parsable()
{
    std::shared_ptr<osi::automata::nlp> parser;
    bool ok = osi::automata::nlp::of(bstyle::nlexer_rule(),
                                     bstyle::syntaxer_rule(),
                                     parser);
    assert(ok);
    assert(parser != nullptr);
    std::cout << "[PASS] nlp_parsable\n";
}

void test_real__file__()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    {
        auto guard = bstyle::parse_wrapper::with_current_file("real__file__.txt");
        assert(parse(io, bstyle_test_data::real__file__, sim));
    }
    assert_execute_without_errors(sim);
    assert(io.output() == "real__file__.txt");
    std::cout << "[PASS] real__file__\n";
}

void test_predefined_def()
{
    primitive::simulator sim;
    assert(bstyle::with_default_functions().compile(bstyle_test_data::predefined_def, sim));
    assert_execute_without_errors(sim);
    std::cout << "[PASS] predefined_def\n";
}

void test_func_name_with_dot()
{
    primitive::console_io::test_wrapper io;
    primitive::simulator sim;
    assert(parse(io, bstyle_test_data::func_name_with_dot, sim));
    assert_execute_without_errors(sim);
    assert(io.output().length() == 2);
    assert(static_cast<int>(io.output()[0]) == 1);
    assert(static_cast<int>(io.output()[1]) == 2);
    std::cout << "[PASS] func_name_with_dot\n";
}

void test_errors_dot_is_disallowed_as_the_end_of_name()
{
    primitive::simulator sim;
    bool ok = bstyle::with_default_functions().compile(bstyle_test_data::errors_dot_is_disallowed_as_the_end_of_name, sim);
    assert(!ok);
    std::cout << "[PASS] errors_dot_is_disallowed_as_the_end_of_name\n";
}

int main()
{
    std::cout << "Starting bstyle tests...\n";
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

    std::cout << "\nAll bstyle tests passed successfully!\n";
    return 0;
}
