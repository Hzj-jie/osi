#pragma once
#include <string>
#include "../test_data_loader.hpp"

namespace osi::compiler::b3style_test_data
{
    inline const std::string func_kw = osi::compiler::test_data::load("b3style", "__func__.txt");
    inline const std::string destruction_in_declaration = osi::compiler::test_data::load("b3style", "destruction_in_declaration.txt");
    inline const std::string destruction_in_definition = osi::compiler::test_data::load("b3style", "destruction_in_definition.txt");
    inline const std::string errors_define_class_constructor_for_non_class = osi::compiler::test_data::load("b3style", "errors/define-class-constructor-for-non-class.txt");
    inline const std::string local_struct_sibling_reuse = osi::compiler::test_data::load("b3style", "local_struct_sibling_reuse.txt");
    inline const std::string block_typedef_shadowing = osi::compiler::test_data::load("b3style", "block_typedef_shadowing.txt");
    inline const std::string errors_local_struct_encapsulation = osi::compiler::test_data::load("b3style", "errors/local_struct_encapsulation.txt");
}
