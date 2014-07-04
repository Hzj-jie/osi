
#pragma once
#include <iostream>
#include <vector>
#include "error_type.hpp"
#include "error_writer.hpp"
#include "../template/singleton.hpp"
#include "../envs/deploys.hpp"
using namespace std;

namespace
{
    static vector<ierror_writer> writers { };
    static const class default_writers
    {
    private:
        default_writers()
        {
            writers.push_back(error_type_selected_error_writer(
                                    console_error_writer(),
                                    { error_type::critical,
                                      error_type::exclamation,
                                      error_type::system,
                                      error_type::other }));
            // TODO: add file name 
            writers.push_back(file_error_writer());
        }

        CONST_SINGLETON(default_writers);
    }& default_writers_instance = default_writers::instance();
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

