
#pragma once
#include "../../../formation/concurrent/qless3.hpp"
#include "../../../utt/icase.hpp"
#include "slimqless_test_template.hpp"

class qless3_test : public slimqless_test_template<qless3>
{
    DEFINE_CASE(qless3_test);
};
REGISTER_CASE(qless3_test);

