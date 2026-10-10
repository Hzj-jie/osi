#pragma once
#include <string>
#include "../test_data_loader.hpp"

namespace logic_test::import_cases
{
    inline const std::string case1 = osi::compiler::test_data::load("logic", "case1.txt");
    inline const std::string case2 = osi::compiler::test_data::load("logic", "case2.txt");
    inline const std::string case3 = osi::compiler::test_data::load("logic", "case3.txt");
    inline const std::string case4 = osi::compiler::test_data::load("logic", "case4.txt");
    inline const std::string heap = osi::compiler::test_data::load("logic", "heap.txt");
    inline const std::string address_of = osi::compiler::test_data::load("logic", "address_of.txt");
    inline const std::string address_of_address_of_param = osi::compiler::test_data::load("logic", "address_of_address_of_param.txt");
    inline const std::string address_of_param = osi::compiler::test_data::load("logic", "address_of_param.txt");
    inline const std::string address_of_ref = osi::compiler::test_data::load("logic", "address_of_ref.txt");
    inline const std::string copy_address_of = osi::compiler::test_data::load("logic", "copy_address_of.txt");
    inline const std::string callee_ref = osi::compiler::test_data::load("logic", "callee_ref.txt");
    inline const std::string callee_ref2 = osi::compiler::test_data::load("logic", "callee_ref2.txt");
}
