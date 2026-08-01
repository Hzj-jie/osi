
#pragma once
#include <filesystem>
#include <iostream>
#include "../app_info/k_assert.hpp"

#if defined(__CYGWIN__) || defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #define OS_POSIX
#elif defined(_WIN32) || defined(_WIN64)
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
    using namespace std::filesystem;
    path c(p);
    c /= v;
    p = c.native();
    return p;
}

template <typename RT, typename T>
static std::filesystem::path::string_type append_path(const RT& p, T&& v)
{
    using namespace std::filesystem;
    path c(p);
    c /= v;
    return c.native();
}

template <typename T>
static std::filesystem::path::string_type path_string(T&& v)
{
    using namespace std::filesystem;
    path c(v);
    return c.native();
}

#if defined(_MSC_VER)
#define WIDE_STRING(x) L#x
#define INLINE __forceinline
#else
#define WIDE_STRING(x) L""#x
#define INLINE inline
#endif

#if defined(__GNUC__)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

#include <atomic>

inline void compiler_barrier()
{
    std::atomic_signal_fence(std::memory_order_seq_cst);
}

inline void compiler_fence()
{
    compiler_barrier();
}

inline void memory_barrier()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

inline void memory_fence()
{
    memory_barrier();
}

inline void full_ordering_barrier()
{
    compiler_barrier();
    memory_barrier();
}

inline void full_ordering_fence()
{
    full_ordering_barrier();
}

