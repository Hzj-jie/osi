#pragma once
#include <string>
#include "../test_data_loader.hpp"

namespace osi::compiler::bstyle_test_data
{
    inline const std::string case1 = osi::compiler::test_data::load("bstyle", "case1.txt");
    inline const std::string case2 = osi::compiler::test_data::load("bstyle", "case2.txt");
    inline const std::string global_variable = osi::compiler::test_data::load("bstyle", "global_variable.txt");
    inline const std::string overload_function = osi::compiler::test_data::load("bstyle", "overload_function.txt");
    inline const std::string single_level_struct = osi::compiler::test_data::load("bstyle", "single_level_struct.txt");
    inline const std::string nested_struct = osi::compiler::test_data::load("bstyle", "nested_struct.txt");
    inline const std::string function_struct = osi::compiler::test_data::load("bstyle", "function_struct.txt");
    inline const std::string return_struct = osi::compiler::test_data::load("bstyle", "return_struct.txt");
    inline const std::string call_struct_on_heap = osi::compiler::test_data::load("bstyle", "call_struct_on_heap.txt");
    inline const std::string for_loop = osi::compiler::test_data::load("bstyle", "for_loop.txt");
    inline const std::string empty_struct_overloads = osi::compiler::test_data::load("bstyle", "empty_struct_overloads.txt");
    inline const std::string delegate_str = osi::compiler::test_data::load("bstyle", "delegate.txt");
    inline const std::string statement = osi::compiler::test_data::load("bstyle", "statement.txt");
    inline const std::string real__file__ = osi::compiler::test_data::load("bstyle", "real__file__.txt");
    inline const std::string predefined_def = osi::compiler::test_data::load("bstyle", "predefined-def.txt");
    inline const std::string func_name_with_dot = osi::compiler::test_data::load("bstyle", "func_name_with_dot.txt");
    inline const std::string errors_dot_is_disallowed_as_the_end_of_name = osi::compiler::test_data::load("bstyle", "errors/dot-is-disallowed-as-the-end-of-name.txt");
}
