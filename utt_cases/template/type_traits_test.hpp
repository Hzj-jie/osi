
#pragma once
#include "../../template/type_traits.hpp"
#include "../../utt/icase.hpp"
#include <string>
#include "../../app_info/trace.hpp"
#include <iostream>

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

    class partial_comparable
    {
    public:
        bool operator<(partial_comparable&) const { return false; }
        bool operator==(partial_comparable&) { return false; }
        bool operator!=(const partial_comparable&) const { return false; }
    };

    bool partial_comparable_case()
    {
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_equal_const_const);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_equal_const_variant);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_equal_variant_const);
        utt_assert.is_true(operator_overloads<partial_comparable>::has_operator_equal_variant_variant);

        utt_assert.is_true(operator_overloads<partial_comparable>::has_operator_not_equal_const_const);
        utt_assert.is_true(operator_overloads<partial_comparable>::has_operator_not_equal_const_variant);
        utt_assert.is_true(operator_overloads<partial_comparable>::has_operator_not_equal_variant_const);
        utt_assert.is_true(operator_overloads<partial_comparable>::has_operator_not_equal_variant_variant);

        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_less_const_const);
        utt_assert.is_true(operator_overloads<partial_comparable>::has_operator_less_const_variant);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_less_variant_const);
        utt_assert.is_true(operator_overloads<partial_comparable>::has_operator_less_variant_variant);

        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_less_or_equal_const_const);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_less_or_equal_const_variant);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_less_or_equal_variant_const);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_less_or_equal_variant_variant);

        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_more_const_const);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_more_const_variant);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_more_variant_const);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_more_variant_variant);

        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_more_or_equal_const_const);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_more_or_equal_const_variant);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_more_or_equal_variant_const);
        utt_assert.is_false(operator_overloads<partial_comparable>::has_operator_more_or_equal_variant_variant);

        return true;
    }

    class not_comparable { };

    bool compare_fail_case()
    {
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_equal_const_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_equal_const_variant);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_equal_variant_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_equal_variant_variant);

        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_not_equal_const_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_not_equal_const_variant);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_not_equal_variant_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_not_equal_variant_variant);

        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_less_const_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_less_const_variant);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_less_variant_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_less_variant_variant);

        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_less_or_equal_const_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_less_or_equal_const_variant);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_less_or_equal_variant_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_less_or_equal_variant_variant);

        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_more_const_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_more_const_variant);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_more_variant_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_more_variant_variant);

        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_more_or_equal_const_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_more_or_equal_const_variant);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_more_or_equal_variant_const);
        utt_assert.is_false(operator_overloads<not_comparable>::has_operator_more_or_equal_variant_variant);

        return true;
    }

    template <typename T1, typename T2>
    bool left_shift_case()
    {
        utt_assert.is_true(operator_overloads<T1, T2>::has_operator_left_shift_variant_const);
        utt_assert.is_true(operator_overloads<T1, T2>::has_operator_left_shift_variant_variant);
        return true;
    }

public:
    bool run() override
    {
        return compare_case<int>() &&
               compare_case<unsigned int>() &&
               compare_case<std::string>() &&
               compare_fail_case() &&
               partial_comparable_case() &&
               left_shift_case<std::ostream, int>() &&
               left_shift_case<std::ostream, std::string>();
    }

    DEFINE_CASE(has_operator_test);
};
REGISTER_CASE(has_operator_test);

