
#ifndef __OSI_ASSERT__
#define __OSI_ASSERT__

#include "error_handle.hpp"
#include "k_assert.hpp"
#include "../utils/strutils.hpp"
#include <utility>

namespace __assert_private
{
    const static error_type assert_err_type = error_type::critical;

    template <typename... Args>
    static void assert_raise_error(Args&&... args)
    {
        raise_error(assert_err_type, "ASSERTION FAILED: ", std::forward<Args>(args)...);
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
    return x || __assert_private::assert_failed(x);
}

template <typename... Args>
static bool assert(bool x, Args&&... args)
{
    return x || __assert_private::assert_failed(std::forward<Args>(args)...);
}

#endif

#undef assert

