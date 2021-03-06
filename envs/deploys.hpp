
#pragma once
#include <string>
#include <boost/filesystem.hpp>
#include "exeinfo.hpp"
#include "../template/singleton.hpp"
#include "../const/character.hpp"
#include "../utils/auto_removed_folder.hpp"
#include "../utils/uuid.hpp"
#include "os.hpp"
#include "git.hpp"
#include "nowadays.hpp"

const static class deploys_t
{
private:
    typedef boost::filesystem::path::string_type path_string;
    typedef boost::filesystem::path::value_type path_char;
    path_string _service_name;
    path_string _deploys_folder;
    path_string _apps_folder;
    path_string _counter_folder;
    path_string _data_folder;
    path_string _log_folder;
    path_string _temp_folder;
    path_string _service_data_folder;
    path_string _service_log_folder;
    path_string _service_counter_folder;
    path_string _service_temp_folder;
    path_string _application_info_output_filename;

    CONST_STATIC_PATH_STRING(deploys_folder_name, deploys);
    CONST_STATIC_PATH_STRING(apps_folder_name, apps);
    CONST_STATIC_PATH_STRING(counter_folder_name, counter);
    CONST_STATIC_PATH_STRING(data_folder_name, data);
    CONST_STATIC_PATH_STRING(log_folder_name, log);
    CONST_STATIC_PATH_STRING(temp_folder_name, temp);

    deploys_t()
    {
        using namespace boost::filesystem;
        {
            path p(exeinfo.path());
            if(!p.empty() &&
               !p.parent_path().empty() &&
               p.parent_path().has_filename())
            {
                _service_name = p.parent_path().filename().native();
            }
            else if(!p.empty() &&
                    p.has_filename())
            {
                _service_name = p.filename().native();
            }
            else
            {
                _service_name = ::path_string("unknown");
            }
            if(!p.empty() &&
               !p.parent_path().empty() &&
               !p.parent_path().parent_path().empty() &&
               p.parent_path().parent_path().has_filename() &&
               p.parent_path().parent_path().filename() == apps_folder_name() &&
               !p.parent_path().parent_path().parent_path().empty() &&
               p.parent_path().parent_path().parent_path().has_filename() &&
               p.parent_path().parent_path().parent_path().filename() == deploys_folder_name())
            {
                _deploys_folder = p.parent_path().parent_path().parent_path().native();
            }
            else
            {
                _deploys_folder = p.root_path().native();
            }
        }

#define append(x) { _##x = (path(_deploys_folder) / x##_name()).native(); }
        append(apps_folder);
        append(counter_folder);
        append(data_folder);
        append(log_folder);
        append(temp_folder);
#undef append
#define append(x) { _service_##x = (path(_##x) / _service_name).native(); }
        append(temp_folder);
        _service_temp_folder = (path(_service_temp_folder) / uuid_str()).native();
        append(data_folder);
        append(log_folder);
        append(counter_folder);
#undef append
        {
            path p(exeinfo.name());
            p += character.underscore;
            p += git.short_commit();
            p += character.underscore;
            p += nowadays.short_time(character.underscore,
                                     character.minus_sign,
                                     character.minus_sign);
            _application_info_output_filename = p.native();
        }
    }
public:
#define return_value(x) \
    const path_string& x() const { \
        return _##x; }
    
    return_value(service_name);
    return_value(deploys_folder);
    return_value(apps_folder);
    return_value(counter_folder);
    return_value(data_folder);
    return_value(log_folder);
    return_value(service_data_folder);
    return_value(service_log_folder);
    return_value(service_counter_folder);
    return_value(application_info_output_filename);
#undef return_value
    const path_string& temp_folder() const
    {
        boost::filesystem::create_directory(_temp_folder);
        return _temp_folder;
    }

    const path_string& service_temp_folder() const
    {
        static const auto_removed_folder _auto_removed_temp_folder(_service_temp_folder);
        return _service_temp_folder;
    }

    template <typename T>
    path_string append_application_info_output_filename(const T& v) const
    {
        using namespace boost::filesystem;
        path p(application_info_output_filename());
        p += v;
        return p.native();
    }
CONST_SINGLETON(deploys_t);
}& deploys = deploys_t::instance();

