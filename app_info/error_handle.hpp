
#include <iostream>
#include <vector>
#include "error_type.hpp"
#include "error_writer.hpp"
using namespace std;

namespace
{
    vector<ierror_writer> writers { };
}

static void raise_error(error_type err_type,
                        char err_type_char,
                        const char* const err_msg,
                        const char* const file,
                        const int line,
                        const char* const func)
{
}

static void raise_error(error_type err_type,
                        char err_type_char,
                        const string& err_msg,
                        const char* const file,
                        const int line,
                        const char* const func)
{
    raise_error(err_type,
                err_type_char,
                err_msg.c_str(),
                file,
                line,
                func);
}

