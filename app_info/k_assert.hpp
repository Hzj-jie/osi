
#pragma once
#include <iostream>
#include <assert.h>

static bool assert_failed()
{
    assert(0);
    return false;
}
#undef assert

#define k_assert(x) { \
    using namespace std; \
    if(!(x)) { \
        cout << "ASSERT FAILED: " \
             << #x \
             << " @ " \
             << BOOST_CURRENT_FUNCTION \
             << " @ " \
             << __FILE__ \
             << ":" \
             << __LINE__ \
             << endl; \
        assert_failed(); } }

