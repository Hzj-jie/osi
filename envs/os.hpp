
#pragma once
#include <boost/predef.h>
#include <boost/filesystem.hpp>
#include <boost/system/api_config.hpp>
// include BOOST_WINDOWS_API and BOOSE_POSIX_API
#include "../template/singleton.hpp"
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

template <typename T>
static void append_path(boost::filesystem::path::string_type& p, T&& v)
{
    using namespace boost::filesystem;
    path c(p);
    c /= v;
    p = c.native();
}

template <typename T>
static boost::filesystem::path::string_type append_path(const boost::filesystem::path::string_type& p,
                                                        T&& v)
{
    using namespace boost::filesystem;
    path c(p);
    c /= v;
    return c.native();
}

#if BOOST_COMP_MSVC
#define WIDE_STRING(x) L#x
#define INLINE __forceinline
#else
#define WIDE_STRING(x) L""#x
#define INLINE inline
#endif

#if BOOST_COMP_GNUC
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

