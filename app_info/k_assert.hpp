
#pragma once
#include <iostream>
#include <assert.h>
#include <boost/current_function.hpp>

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
             << BOOST_CURRENT_FUNCTION \
             << " @ " \
             << __FILE__ \
             << ":" \
             << __LINE__ \
             << endl; \
        ::assert_failed(); } }

