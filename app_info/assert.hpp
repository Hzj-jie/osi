
#pragma once
#include "error_handle.hpp"
#include "trace.hpp"
#include "k_assert.hpp"
#include "../utils/strutils.hpp"

namespace
{
    using namespace std;
    const static error_type assert_err_type = error_type::critical;

#define ASSERT_ERROR_MSG(err_msg) strcat("ASSERT FAILED: ", err_msg)
    template <typename T>
    static void assert_raise_error(T&& err_msg)
    {
        raise_error(assert_err_type, ASSERT_ERROR_MSG(err_msg));
    }

    template <typename T>
    static void assert_raise_error(T&& err_msg, const code_position& cp)
    {
        raise_error(assert_err_type, ASSERT_ERROR_MSG(err_msg), cp);
    }
#undef ASSERT_ERROR_MSG

    template <typename T>
    static bool assert_failed(T&& err_msg)
    {
        assert_raise_error(err_msg);
        return ::assert_failed();
    }

    template <typename T>
    static bool assert_failed(T&& err_msg, const code_position& cp)
    {
        assert_raise_error(err_msg, cp);
        return ::assert_failed();
    }
}

#undef assert
static bool assert(bool x)
{
    return x || assert_failed(x);
}

static bool assert(bool x, const code_position& cp)
{
    return x || assert_failed(x, cp);
}

template <typename T>
static bool assert(bool x, T&& err_msg)
{
    return x || assert_failed(err_msg);
}

template <typename T>
static bool assert(bool x, T&& err_msg, const code_position& cp)
{
    return x || assert_failed(err_msg, cp);
}

