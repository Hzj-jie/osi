
#include <string>
#include <boost/filesystem.hpp>
#include "exeinfo.hpp"
#include "../template/singleton.hpp"

const static class deploys_t
{
private:
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
        if(path(exeinfo.path()).parent_path().string() == apps_folder_name())
        {
        }
        else
        {
        }
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
    }
CONST_SINGLETON(deploys_t);
}& deploys = deploys_t::instance();

