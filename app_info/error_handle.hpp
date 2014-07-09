
#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/current_function.hpp>
#include <boost/filesystem.hpp>
#include "error_type.hpp"
#include "error_writer.hpp"
#include "../template/singleton.hpp"
#include "../envs/deploys.hpp"
#include "../envs/os.hpp"

namespace
{
    using namespace std;
    using namespace error_handle;
    static vector<ierror_writer*> writers { };
    static const class default_writers
    {
    public:
        ~default_writers()
        {
            for(size_t i = 0; i < writers.size(); i++)
                delete writers[i];
            writers.clear();
        }
    private:
        default_writers()
        {
            writers.push_back(new error_type_selected_error_writer<console_error_writer>
                             (console_error_writer(),
                              { error_type::critical,
                                error_type::exclamation,
                                error_type::system,
                                error_type::other }));
        }

        CONST_SINGLETON(default_writers);
    }& default_writers_instance = default_writers::instance();
}

namespace error_handle
{
    using namespace std;
    static void add_writer(ierror_writer* writer)
    {
        writers.push_back(writer);
    }

    static void enable_default_file_error_writer()
    {
        using namespace error_handle;
        using namespace boost::filesystem;
        create_directory(deploys.log_folder());
        add_writer(new file_error_writer(
                          append_path(deploys.log_folder(),
                                      deploys.append_application_info_output_filename(".log"))));
    }
}

static void raise_error(error_type err_type,
                        char err_type_char,
                        const char* const err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    using namespace std;
    ostringstream os;
    os << error_type_to_char(err_type, err_type_char)
       << ", "
       << err_msg;
    if(file != nullptr &&
       line > 0 &&
       func != nullptr)
    {
        os << " @ "
           << file
           << ":"
           << line
           << "@"
           << func;
    }
    os << endl;
    for(size_t i = 0; i < writers.size(); i++)
    {
        k_assert(writers[i] != nullptr);
        writers[i]->write(os.str());
    }
}

static void raise_error(error_type err_type,
                        char err_type_char,
                        const string& err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    raise_error(err_type,
                err_type_char,
                err_msg.c_str(),
                file,
                line,
                func);
}

static void raise_error(error_type err_type,
                        char err_type_char,
                        const std::ostringstream& err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    raise_error(err_type,
                err_type_char,
                err_msg.str(),
                file,
                line,
                func);
}

static void raise_error(error_type err_type,
                        const std::string& err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    raise_error(err_type, character.null, err_msg, file, line, func);
}

static void raise_error(error_type err_type,
                        const char* const err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    raise_error(err_type, character.null, err_msg, file, line, func);
}

static void raise_error(error_type err_type,
                        const std::ostringstream& err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    raise_error(err_type, character.null, err_msg, file, line, func);
}

static void raise_error(const std::string& err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    raise_error(error_type::information, err_msg, file, line, func);
}

static void raise_error(const char* const err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    raise_error(error_type::information, err_msg, file, line, func);
}

static void raise_error(const std::ostringstream& err_msg,
                        const char* const file = nullptr,
                        const int line = 0,
                        const char* const func = nullptr)
{
    raise_error(error_type::information, err_msg, file, line, func);
}

#define ERROR_POS __FILE__, __LINE__, BOOST_CURRENT_FUNCTION

