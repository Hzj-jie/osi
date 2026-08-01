#pragma once
#include <iostream>
#include <cassert>

#if defined(__GNUC__) || defined(__clang__)
#define OSI_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define OSI_CURRENT_FUNCTION __FUNCSIG__
#else
#define OSI_CURRENT_FUNCTION __func__
#endif

static bool assert_failed()
{
    assert(0);
    return false;
}
#undef assert

#define k_assert(x) { \
    using namespace std; \
    if(!(x)) { \
        cout << "ASSERTION FAILED: " \
             << #x \
             << " @ " \
             << OSI_CURRENT_FUNCTION \
             << " @ " \
             << __FILE__ \
             << ":" \
             << __LINE__ \
             << endl; \
        ::assert_failed(); } }
