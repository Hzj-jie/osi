
#pragma once
#include <string>
#include <boost/filesystem.hpp>
#include "exeinfo.hpp"
#include "../template/singleton.hpp"
#include "../utils/sweeper.hpp"

const static class deploys_t
{
private:
    std::string _service_name;
    std::string _deploys_folder;
    std::string _apps_folder;
    std::string _counter_folder;
    std::string _data_folder;
    std::string _log_folder;
    std::string _temp_folder;
    std::string _service_data_folder;

    CONST_STATIC_STRING(deploys_folder_name, deploys);
    CONST_STATIC_STRING(apps_folder_name, apps);
    CONST_STATIC_STRING(counter_folder_name, counter);
    CONST_STATIC_STRING(data_folder_name, data);
    CONST_STATIC_STRING(log_folder_name, log);
    CONST_STATIC_STRING(temp_folder_name, temp);

    deploys_t()
    {
        using namespace boost::filesystem;
        using namespace std;
        path p(exeinfo.path());
        if(!p.empty() &&
           !p.parent_path().empty() &&
           p.parent_path().has_filename())
        {
            _service_name = p.parent_path().filename();
        }
        else if(!p.empty() &&
                p.has_filename())
        {
            _service_name = p.filename();
        }
        else
        {
            _service_name = "unknown";
        }
        if(!p.empty() &&
           !p.parent_path().empty() &&
           !p.parent_path().parent_path().empty() &&
           !p.parent_path().parent_path().has_filename() &&
           p.parent_path().parent_path().filename() == apps_folder_name() &&
           !p.parent_path().parent_path().parent_path().empty())
        {
            _deploys_folder = p.parent_path().parent_path().parent_path().string();
        }
        else
        {
            _deploys_folder = p.root_path().string();
        }

#define append(x) _##x = (path(_deploys_folder) / x##_name()).string()
        append(app_folder);
        append(counter_folder);
        append(data_folder);
        append(log_folder);
        append(temp_folder);
#undef append
        _service_data_folder = (path(_data_folder) / _service_name).string();
    }
public:
#define return_value(x) \
    const std::string& x() { \
        return _x; }
    
    return_value(deploys_folder);
    return_value(app_folder);
    return_value(counter_folder);
    return_value(data_folder);
    return_value(log_folder);
    return_value(service_data_folder);
#undef return_value
    const std::string& temp_folder()
    {
        using namespace boost::filesystem;
        static const sweeper s(
                [this]()
                {
                    create_directory(_temp_folder);
                },
                [this]()
                {
                });
        return _temp_folder;
    }
CONST_SINGLETON(deploys_t);
}& deploys = deploys_t::instance();

