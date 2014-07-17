
#pragma once
#include <string>
#include "os.hpp"
#include "../template/singleton.hpp"
#include <boost/filesystem.hpp>

#if defined(OS_WINDOWS)
#include <windows.h>
namespace
{
    std::string getexepath()
    {
        char result[MAX_PATH];
        return std::string(result, GetModuleFileName(nullptr, result, MAX_PATH));
    }
}
#elif defined(OS_POSIX)
#include <limits.h>
#include <unistd.h>
namespace
{
    std::string getexepath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, count > 0 ? count : 0);
    }
}
#endif

const static class exeinfo_t
{
private:
    std::string _path;
    std::string _name;
    std::string _directory;

    exeinfo_t()
    {
        _path = getexepath();
        namespace bt = boost::filesystem;
        bt::path p(_path);
        if(!p.empty() && p.has_filename())
        {
            _name = p.filename().string();
            _directory = p.parent_path().string();
        }
        else
        {
            _name = "UNKNOWN";
            _directory = "UNKNOWN";
        }
    }
public:
#define return_value(x) \
    const std::string& x() const { \
        return _##x; }
    return_value(path);
    return_value(name);
    return_value(directory);
#undef return_value
CONST_SINGLETON(exeinfo_t);
}& exeinfo = exeinfo_t::instance();

