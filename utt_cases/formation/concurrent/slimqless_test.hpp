
#pragma once
#include "slimqless_test_template.hpp"
#include "../../../formation/concurrent/slimqless.hpp"
#include "../../../utt/icase.hpp"

class slimqless_test : public slimqless_test_template<slimqless>
{
    DEFINE_CASE(slimqless_test);
};

REGISTER_CASE(slimqless_test);

