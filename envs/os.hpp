
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

static void append_path(boost::filesystem::path::string_type& p, const std::string& v)
{
    using namespace boost::filesystem;
    path c(p);
    c /= v;
    p = c.native();
}

#if BOOST_COMP_MSVC
#define WIDE_STRING(x) L#x
#else
#define WIDE_STRING(x) L""#x
#endif

