
#pragma once
#include <string.h>
#include <string>
#include "../template/singleton.hpp"
#include "utt_error_handle.hpp"
#include "../app_info/trace.hpp"
#include "../const/character.hpp"
#include <atomic>
#include <stdint.h>

namespace utt
{
    const static class assert_t
    {
    private:
        mutable atomic<uint32_t> _failure_count;
#define UTT_ASSERT(v, cp) { \
            if(v) return true; \
            else { \
                _failure_count++; \
                std::string err_msg = "UTT ASSERT FAILED: "#v; \
                if(cp == nullptr) \
                    utt_raise_error(err_msg); \
                else \
                    utt_raise_error(err_msg, *cp); \
                return false; } }
    public:
        uint32_t failure_count() const
        {
            return _failure_count;
        }

        template <typename T1, typename T2>
        bool equal(T1&& i, T2&& j) const
        {
            UTT_ASSERT(i == j, EMPTY_CODE_POSITION());
        }

        template <typename T1, typename T2>
        bool equal(T1&& i, T2&& j, const code_position& cp) const
        {
            UTT_ASSERT(i == j, &cp);
        }

        bool equal(const char* i, const char* j) const
        {
            UTT_ASSERT(strcmp(i, j) == 0, EMPTY_CODE_POSITION());
        }

        bool equal(const char* i, const char* j, const code_position& cp) const
        {
            UTT_ASSERT(strcmp(i, j) == 0, &cp);
        }

        template <typename T1, typename T2>
        bool not_equal(T1&& i, T2&& j) const
        {
            UTT_ASSERT(i != j, EMPTY_CODE_POSITION());
        }

        template <typename T1, typename T2>
        bool not_equal(T1&& i, T2&& j, const code_position& cp) const
        {
            UTT_ASSERT(i != j, &cp);
        }

        bool not_equal(const char* i, const char* j) const
        {
            UTT_ASSERT(strcmp(i, j) != 0, EMPTY_CODE_POSITION());
        }

        bool not_equal(const char* i, const char* j, const code_position& cp) const
        {
            UTT_ASSERT(strcmp(i, j) != 0, &cp);
        }

        template <typename T1, typename T2>
        bool less(T1&& i, T2&& j) const
        {
            UTT_ASSERT(i < j, EMPTY_CODE_POSITION());
        }

        template <typename T1, typename T2>
        bool less(T1&& i, T2&& j, const code_position& cp) const
        {
            UTT_ASSERT(i < j, &cp);
        }

        bool less(const char* i, const char* j) const
        {
            UTT_ASSERT(strcmp(i, j) < 0, EMPTY_CODE_POSITION());
        }

        bool less(const char* i, const char* j, const code_position& cp) const
        {
            UTT_ASSERT(strcmp(i, j) < 0, &cp);
        }

        template <typename T1, typename T2>
        bool less_or_equal(T1&& i, T2&& j) const
        {
            UTT_ASSERT(i <= j, EMPTY_CODE_POSITION());
        }

        template <typename T1, typename T2>
        bool less_or_equal(T1&& i, T2&& j, const code_position& cp) const
        {
            UTT_ASSERT(i <= j, &cp);
        }

        bool less_or_equal(const char* i, const char* j) const
        {
            UTT_ASSERT(strcmp(i, j) <= 0, EMPTY_CODE_POSITION());
        }

        bool less_or_equal(const char* i, const char* j, const code_position& cp) const
        {
            UTT_ASSERT(strcmp(i, j) <= 0, &cp);
        }

        template <typename T1, typename T2>
        bool more(T1&& i, T2&& j) const
        {
            UTT_ASSERT(i > j, EMPTY_CODE_POSITION());
        }

        template <typename T1, typename T2>
        bool more(T1&& i, T2&& j, const code_position& cp) const
        {
            UTT_ASSERT(i > j, &cp);
        }

        bool more(const char* i, const char* j) const
        {
            UTT_ASSERT(strcmp(i, j) > 0, EMPTY_CODE_POSITION());
        }

        bool more(const char* i, const char* j, const code_position& cp) const
        {
            UTT_ASSERT(strcmp(i, j) > 0, &cp);
        }

        template <typename T1, typename T2>
        bool more_or_equal(T1&& i, T2&& j) const
        {
            UTT_ASSERT(i >= j, EMPTY_CODE_POSITION());
        }

        template <typename T1, typename T2>
        bool more_or_equal(T1&& i, T2&& j, const code_position& cp) const
        {
            UTT_ASSERT(i >= j, &cp);
        }

        bool more_or_equal(const char* i, const char* j) const
        {
            UTT_ASSERT(strcmp(i, j) >= 0, EMPTY_CODE_POSITION());
        }

        bool more_or_equal(const char* i, const char* j, const code_position& cp) const
        {
            UTT_ASSERT(strcmp(i, j) >= 0, &cp);
        }

        template <typename T>
        bool is_null(const T* i) const
        {
            UTT_ASSERT(i == nullptr, EMPTY_CODE_POSITION());
        }

        template <typename T>
        bool is_null(const T* i, const code_position& cp) const
        {
            UTT_ASSERT(i == nullptr, &cp);
        }

        template <typename T>
        bool is_not_null(const T* i) const
        {
            UTT_ASSERT(i != nullptr, EMPTY_CODE_POSITION());
        }

        template <typename T>
        bool is_not_null(const T* i, const code_position& cp) const
        {
            UTT_ASSERT(i != nullptr, &cp);
        }

        template <typename T>
        bool is_true(T&& i) const
        {
            UTT_ASSERT(i, EMPTY_CODE_POSITION());
        }

        template <typename T>
        bool is_true(T&& i, const code_position& cp) const
        {
            UTT_ASSERT(i, &cp);
        }

        template <typename T>
        bool is_false(T&& i) const
        {
            UTT_ASSERT(!(i), EMPTY_CODE_POSITION());
        }

        template <typename T>
        bool is_false(T&& i, const code_position& cp) const
        {
            UTT_ASSERT(!(i), &cp);
        }
#undef UTT_ASSERT
    private:
        assert_t() { }
        CONST_SINGLETON(assert_t);
    }& assert = assert_t::instance();
}

static const utt::assert_t& utt_assert = utt::assert;

