
#pragma once
#include <string>
#include "os.hpp"
#include "../template/singleton.hpp"
#include <boost/filesystem.hpp>
#include <stdint.h>

#if defined(OS_WINDOWS)
#include <windows.h>
namespace __exeinfo_private
{
    static std::string getexepath()
    {
        char result[MAX_PATH];
        return std::string(result, GetModuleFileName(nullptr, result, MAX_PATH));
    }

    static int64_t get_current_process_id()
    {
        // from uint32_t to int64_t
        return GetCurrentProcessId();
    }
}
#elif defined(OS_POSIX)
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
namespace __exeinfo_private
{
    static std::string getexepath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, count > 0 ? count : 0);
    }

    static int64_t get_current_process_id()
    {
        // from int32_t to int64_t
        return getpid();
    }
}
#endif

const static class exeinfo_t
{
private:
    std::string _path;
    std::string _name;
    std::string _directory;
    int64_t _id;

    exeinfo_t()
    {
        _path = __exeinfo_private::getexepath();
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
        _id = __exeinfo_private::get_current_process_id();
    }
public:
#define return_value(x) \
    const std::string& x() const { \
        return _##x; }
    return_value(path);
    return_value(name);
    return_value(directory);
#undef return_value
    const int64_t id() const
    {
        return _id;
    }

CONST_SINGLETON(exeinfo_t);
}& exeinfo = exeinfo_t::instance();

