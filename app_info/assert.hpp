
#include "error_handle.hpp"
#include <string>

namespace
{
    using namespace std;
    static void assert_raise_error(const char* const msg,
                                   const char* const file,
                                   const int line,
                                   const char* const func)
    {
    }

    static void assert_raise_error(const string& msg,
                                   const char* const file,
                                   const int line,
                                   const char* const func)
    {
    }

    static void assert_raise_error(const ostringstream& msg,
                                   const char* const file,
                                   const int line,
                                   const char* const func)
    {
    }

    static bool assert_failed(const char* const msg,
                              const char* const file,
                              const int line,
                              const char* const func)
}

