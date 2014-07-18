
#pragma once
#include <string.h>
#include <string>
#include "utt_error_handle.hpp"
#include "../const/character.hpp"
#include <atomic>
#include <stdint.h>
#include "../template/singleton.hpp"

namespace utt
{
    class assert_t
    {
    private:
        std::string case_name;
        SINGLETON_FUNC(std::atomic<uint32_t>, f, (0));

        template <typename... Args>
        bool utt_assert(bool v, Args&&... args) const
        {
            if(v) return true;
            else
            {
                f()++;
                utt_raise_error("UTT ", case_name, " ASSERT FAILED: ", std::forward<Args>(args)...);
                return false;
            }
        }
    public:
        void set_case_name(const std::string& name)
        {
            case_name = name;
        }

        static uint32_t failure_count()
        {
            return f();
        }

#define UTT_ASSERT_TEMP(n, x, m) \
            template <typename T1, typename T2, typename... Args> \
            bool n(T1&& i, T2&& j, Args&&... args) const { \
                return utt_assert(x, #x, std::forward<Args>(args)...); } \
            template <typename... Args> \
            bool n(const char* i, const char* j, Args&&... args) const { \
                return utt_assert(strcmp(i, j) m, "strcmp(i, j) "#m, std::forward<Args>(args)...); }
        UTT_ASSERT_TEMP(equal, i == j, == 0);
        UTT_ASSERT_TEMP(not_equal, i != j, != 0);
        UTT_ASSERT_TEMP(less, i < j, < 0);
        UTT_ASSERT_TEMP(less_or_equal, i <= j, <= 0);
        UTT_ASSERT_TEMP(more, i > j, > 0);
        UTT_ASSERT_TEMP(more_or_equal, i >= j, >= 0);
#undef UTT_ASSERT_TEMP

#define UTT_ASSERT_TEMP(n, x) \
            template <typename T, typename... Args> \
            bool n(const T* i, Args&&... args) const { \
                return utt_assert(i x nullptr, "i "#x" nullptr", std::forward<Args>(args)...); }
        UTT_ASSERT_TEMP(is_null, ==);
        UTT_ASSERT_TEMP(is_not_null, !=);
#undef UTT_ASSERT_TEMP

#define UTT_ASSERT_TEMP(n, x) \
            template <typename T, typename... Args> \
            bool n(T&& i, Args&&... args) const { \
                return utt_assert(i x true, "i "#x" true", std::forward<Args>(args)...); }
        UTT_ASSERT_TEMP(is_true, ==);
        UTT_ASSERT_TEMP(is_false, !=);
#undef UTT_ASSERT_TEMP
    };
}

