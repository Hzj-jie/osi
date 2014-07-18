
#pragma once
#include "error_handle.hpp"
#include "k_assert.hpp"
#include "../utils/strutils.hpp"
#include <utility>

#include "assert_blocker.hpp"

namespace
{
    const static error_type assert_err_type = error_type::critical;

    template <typename... Args>
    static void assert_raise_error(Args&&... args)
    {
        raise_error(assert_err_type, "ASSERT FAILED: ", std::forward<Args>(args)...);
    }

    template <typename... Args>
    static bool assert_failed(Args&&... args)
    {
        assert_raise_error(std::forward<Args>(args)...);
        return ::assert_failed();
    }
}

#undef assert
static bool assert(bool x)
{
    return x || assert_failed(x);
}

template <typename... Args>
static bool assert(bool x, Args&&... args)
{
    return x || assert_failed(std::forward<Args>(args)...);
}

