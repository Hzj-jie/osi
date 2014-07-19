
#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/current_function.hpp>
#include <boost/filesystem.hpp>
#include <mutex>
#include "error_type.hpp"
#include "error_writer.hpp"
#include "../template/singleton.hpp"
#include "../envs/deploys.hpp"
#include "../envs/os.hpp"
#include "../utils/strutils.hpp"
#include "../const/character.hpp"
#include "../envs/nowadays.hpp"
#include <utility>

static std::ostream& operator<<(std::ostream& os, const std::ostringstream& v)
{
    os << v.str();
    return os;
}

namespace __error_handle_private
{
    static std::vector<error_handle::ierror_writer*> writers { };

    template <typename... Args>
    static void k_raise_error(error_type err_type,
                              char err_type_char,
                              Args&&... args)
    {
        using namespace std;
        static mutex mtx;
        ostringstream os;
        os << error_handle::error_type_to_char(err_type, err_type_char)
           << character.comma
           << character.blank
           << nowadays.long_time()
           << character.comma
           << character.blank
           << strcat(forward<Args>(args)...)
           << endl;
        for(size_t i = 0; i < writers.size(); i++)
        {
            k_assert(writers[i] != nullptr);
            unique_lock<mutex> lck;
            writers[i] -> write(os.str());
        }
    }
}

namespace error_handle
{
    static void add_writer(ierror_writer* writer)
    {
    	using namespace __error_handle_private;
        writers.push_back(writer);
    }

    static bool remove_writer(ierror_writer* writer)
    {
    	using namespace __error_handle_private;
        for(size_t i = 0; i < writers.size(); i++)
        {
            if(writer == writers[i])
            {
                writers.erase(writers.begin() + i);
                delete writer;
                return true;
            }
        }
        return false;
    }

    static void clear_writers()
    {
    	using namespace __error_handle_private;
		for(size_t i = 0; i < writers.size(); i++)
            delete writers[i];
        writers.clear();
    }

    static void enable_default_file_error_writer()
    {
        using namespace error_handle;
        using namespace boost::filesystem;
    	using namespace __error_handle_private;		
        create_directories(deploys.service_log_folder());
        add_writer(new file_error_writer(
                          append_path(deploys.service_log_folder(),
                                      deploys.append_application_info_output_filename(".log"))));
    }
}

namespace __error_handle_private
{
    static const class default_writers
    {
    public:
        ~default_writers()
        {
            error_handle::clear_writers();
        }
    private:
        default_writers()
        {
            using namespace error_handle;
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

template <typename... Args>
static void raise_error(error_type err_type,
                        char err_type_char,
                        Args&&... args)
{
    __error_handle_private::k_raise_error(err_type,
						                  err_type_char,
						                  std::forward<Args>(args)...);
}

template <typename... Args>
static void raise_error(error_type err_type, Args&&... args)
{
    __error_handle_private::k_raise_error(err_type,
										  character.null,
										  std::forward<Args>(args)...);
}

template <typename... Args>
static void raise_error(Args&&... args)
{
    __error_handle_private::k_raise_error(error_type::information,
										  character.null,
										  std::forward<Args>(args)...);
}

