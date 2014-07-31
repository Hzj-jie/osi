
#pragma once
#include "slimqless_test_template.hpp"
#include "../../../formation/concurrent/slimqless2.hpp"
#include "../../../utt/icase.hpp"

class slimqless2_perf_test_8 : public slimqless_perf_test_8_template<slimqless2>
{
    DEFINE_CASE(slimqless2_perf_test_8);
};
REGISTER_CASE(slimqless2_perf_test_8);

class slimqless2_perf_test_32 : public slimqless_perf_test_32_template<slimqless2>
{
    DEFINE_CASE(slimqless2_perf_test_32);
};
REGISTER_CASE(slimqless2_perf_test_32);

class slimqless2_perf_test_128 : public slimqless_perf_test_128_template<slimqless2>
{
    DEFINE_CASE(slimqless2_perf_test_128);
};
REGISTER_CASE(slimqless2_perf_test_128);

