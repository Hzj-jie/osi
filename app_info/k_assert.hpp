
#pragma once
#include <boost/current_function.hpp>
#include <iostream>

#define k_assert(x) { \
    using namespace std; \
    if(!(x)) { \
        cout << "ASSERT FAILED: " << #x << " @ " << BOOST_CURRENT_FUNCTION << ":" << __FILE__ << "@" << __LINE__ << endl; \
        int i = 0; i /= i; } }

