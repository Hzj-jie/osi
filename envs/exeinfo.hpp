
#include <string>
#include "os.hpp"
using namespace std;

#ifdef OS_WINDOWS
#include <windows.h>
namespace
{
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
    string getexepath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return string(result, count > 0 ? count : 0);
    }
}
#endif

class exeinfo_t
{
private:
    string _path;
public:
    exeinfo_t()
    {
        _path = getexepath();
    }

    const string& path() const
    {
        return _path;
    }
} exeinfo;

