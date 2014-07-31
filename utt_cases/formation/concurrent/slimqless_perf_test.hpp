
#pragma once
#include "slimqless_test_template.hpp"
#include "../../../formation/concurrent/slimqless.hpp"
#include "../../../utt/icase.hpp"

class slimqless_perf_test_8 : public slimqless_perf_test_8_template<slimqless>
{
    DEFINE_CASE(slimqless_perf_test_8);
};
REGISTER_CASE(slimqless_perf_test_8);

class slimqless_perf_test_32 : public slimqless_perf_test_32_template<slimqless>
{
    DEFINE_CASE(slimqless_perf_test_32);
};
REGISTER_CASE(slimqless_perf_test_32);

class slimqless_perf_test_128 : public slimqless_perf_test_128_template<slimqless>
{
    DEFINE_CASE(slimqless_perf_test_128);
};
REGISTER_CASE(slimqless_perf_test_128);

