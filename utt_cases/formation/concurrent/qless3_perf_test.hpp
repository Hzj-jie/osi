
#pragma once
#include "../../../formation/concurrent/qless3.hpp"
#include "slimqless_test_template.hpp"
#include "../../../utt/icase.hpp"

class qless3_perf_test_8 : public slimqless_perf_test_8_template<qless3>
{
    DEFINE_CASE(qless3_perf_test_8);
};
REGISTER_CASE(qless3_perf_test_8);

class qless3_perf_test_32 : public slimqless_perf_test_32_template<qless3>
{
    DEFINE_CASE(qless3_perf_test_32);
};
REGISTER_CASE(qless3_perf_test_32);

class qless3_perf_test_128 : public slimqless_perf_test_128_template<qless3>
{
    DEFINE_CASE(qless3_perf_test_128);
};
REGISTER_CASE(qless3_perf_test_128);
