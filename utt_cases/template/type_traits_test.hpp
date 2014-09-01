
#pragma once
#include "../../template/type_traits.hpp"
#include "../../utt/icase.hpp"
#include <string>

class has_operator_test : public icase
{
private:
    template <typename T>
    bool compare_case()
    {
        utt_assert.is_true(operator_overloads<T>::has_operator_equal_const_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_equal_const_variant);
        utt_assert.is_true(operator_overloads<T>::has_operator_equal_variant_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_equal_variant_variant);

        utt_assert.is_true(operator_overloads<T>::has_operator_not_equal_const_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_not_equal_const_variant);
        utt_assert.is_true(operator_overloads<T>::has_operator_not_equal_variant_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_not_equal_variant_variant);

        utt_assert.is_true(operator_overloads<T>::has_operator_less_const_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_less_const_variant);
        utt_assert.is_true(operator_overloads<T>::has_operator_less_variant_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_less_variant_variant);

        utt_assert.is_true(operator_overloads<T>::has_operator_less_or_equal_const_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_less_or_equal_const_variant);
        utt_assert.is_true(operator_overloads<T>::has_operator_less_or_equal_variant_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_less_or_equal_variant_variant);

        utt_assert.is_true(operator_overloads<T>::has_operator_more_const_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_more_const_variant);
        utt_assert.is_true(operator_overloads<T>::has_operator_more_variant_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_more_variant_variant);

        utt_assert.is_true(operator_overloads<T>::has_operator_more_or_equal_const_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_more_or_equal_const_variant);
        utt_assert.is_true(operator_overloads<T>::has_operator_more_or_equal_variant_const);
        utt_assert.is_true(operator_overloads<T>::has_operator_more_or_equal_variant_variant);

        return true;
    }

public:
    bool run() override
    {
        return compare_case<int>() &&
               compare_case<unsigned int>() &&
               compare_case<std::string>();
    }

    DEFINE_CASE(has_operator_test);
};
REGISTER_CASE(has_operator_test);

