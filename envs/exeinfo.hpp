
#include <string>
#include "os.hpp"
#include "../template/singleton.hpp"

#ifdef OS_WINDOWS
#include <windows.h>
namespace
{
    using namespace std;
    string getexepath()
    {
        char result[MAX_PATH];
        return string(result, GetModuleFileName(nullptr, result, MAX_PATH));
    }
}
#endif
#ifdef OS_POSIX
#include <limits.h>
#include <unistd.h>
namespace
{
    using namespace std;
    string getexepath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return string(result, count > 0 ? count : 0);
    }
}
#endif

const static class exeinfo_t
{
private:
    std::string _path;

    exeinfo_t()
    {
        _path = getexepath();
    }
public:
    const std::string& path() const
    {
        return _path;
    }
CONST_SINGLETON(exeinfo_t);
}& exeinfo = exeinfo_t::instance();

