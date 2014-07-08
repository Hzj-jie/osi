
#pragma once
#include <iostream>

#define k_assert(x) { \
    using namespace std; \
    if(!(x)) { \
        cout << "ASSERT FAILED: " << #x << " @ " << __func__ << ":" << __FILE__ << "@" << __LINE__ << endl; \
        int i = 0; i /= i; } }

