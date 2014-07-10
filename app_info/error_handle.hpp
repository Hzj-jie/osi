
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
#include "trace.hpp"
#include "../utils/strutils.hpp"
#include "../const/character.hpp"

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

    ostream& operator<<(ostream& os, const ostringstream& v)
    {
        os << v;
        return os;
    }

    template <typename T>
    static void raise_error(error_type err_type,
                            char err_type_char,
                            T&& err_msg,
                            const code_position* cp)
    {
        using namespace std;
        ostringstream os;
        os << error_type_to_char(err_type, err_type_char)
           << character.comma
           << character.blank
           << err_msg;
        if(cp != nullptr && (*cp))
        {
            os << character.blank
               << (*cp);
        }
        os << endl;
        for(size_t i = 0; i < writers.size(); i++)
        {
            k_assert(writers[i] != nullptr);
            writers[i] -> write(os.str());
        }
    }
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

template <typename T>
static void raise_error(error_type err_type,
                        char err_type_char,
                        T&& err_msg)
{
    raise_error(err_type,
                err_type_char,
                err_msg,
                nullptr);
}

template <typename T>
static void raise_error(error_type err_type,
                        char err_type_char,
                        T&& err_msg,
                        const code_position& cp)
{
    raise_error(err_type,
                err_type_char,
                err_msg,
                &cp);
}

template <typename T>
static void raise_error(error_type err_type, T&& err_msg)
{
    raise_error(err_type, character.null, err_msg);
}

template <typename T>
static void raise_error(error_type err_type, T&& err_msg, const code_position& cp)
{
    raise_error(err_type, character.null, err_msg, cp);
}

template <typename T>
static void raise_error(T&& err_msg)
{
    raise_error(error_type::information, err_msg);
}

template <typename T>
static void raise_error(T&& err_msg, const code_position& cp)
{
    raise_error(error_type::information, err_msg, cp);
}

#define RAISE_ERROR(err_type, err_msg) { \
    if(error_handle::should_show_code_position(err_type)) \
        raise_error(err_type, err_msg, CODE_POSITION()); \
    else \
        raise_error(err_type, err_msg); }

