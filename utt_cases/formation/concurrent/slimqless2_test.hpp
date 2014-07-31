
#pragma once
#include "slimqless_test_template.hpp"
#include "../../../formation/concurrent/slimqless2.hpp"

class slimqless2_test : public slimqless_test_template<slimqless2>
{
    DEFINE_CASE(slimqless2_test);
};

REGISTER_CASE(slimqless2_test);

