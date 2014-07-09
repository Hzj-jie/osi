
#pragma once
#include <iostream>

#if defined(__func__)
#define __FUNCTION_NAME__ __func__
#elif defined(__FUNCTION__)
#define __FUNCTION_NAME__ __FUNCTION__
#else
#define __FUNCTION_NAME__ "__func__not_supported"
#endif

#define k_assert(x) { \
    using namespace std; \
    if(!(x)) { \
        cout << "ASSERT FAILED: " << #x << " @ " << __FUNCTION_NAME__ << ":" << __FILE__ << "@" << __LINE__ << endl; \
        int i = 0; i /= i; } }

