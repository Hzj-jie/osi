
#pragma once
#include <boost/predef.h>
#include <boost/filesystem.hpp>
#include <boost/system/api_config.hpp>
// include BOOST_WINDOWS_API and BOOSE_POSIX_API
#include "../app_info/k_assert.hpp"

#include <iostream>

#if BOOST_OS_CYGWIN || BOOST_OS_LINUX || BOOST_OS_UNIX
    #define OS_POSIX
#elif BOOST_OS_WINDOWS
    #define OS_WINDOWS
#endif

#ifdef OS_REPORT_BUILDING_ENVIRONMENT
    #ifdef OS_WINDOWS
        #pragma message("build on windows")
    #endif

    #ifdef OS_POSIX
        #pragma message "build on posix"
    #endif
#endif

template <typename RT, typename T>
static RT& append_path(RT& p, T&& v)
{
    using namespace boost::filesystem;
    path c(p);
    c /= v;
    p = c.native();
    return p;
}

template <typename RT, typename T>
static boost::filesystem::path::string_type append_path(const RT& p, T&& v)
{
    using namespace boost::filesystem;
    path c(p);
    c /= v;
    return c.native();
}

template <typename T>
static boost::filesystem::path::string_type path_string(T&& v)
{
    using namespace boost::filesystem;
    path c(v);
    return c.native();
}

#if BOOST_COMP_MSVC
#define WIDE_STRING(x) L#x
#define INLINE __forceinline
#define thread_local __declspec(thread)
#else
#define WIDE_STRING(x) L""#x
#define INLINE inline
#endif

#if BOOST_COMP_GNUC
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

#include <boost/atomic.hpp>

inline void compiler_barrier()
{
    boost::atomic_signal_fence(boost::memory_order_seq_cst);
}

