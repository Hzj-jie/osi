
#include "../template/singleton.hpp"
#include <string>

const static class git_t
{
public:
    CONST_STATIC_STRING(commit, 1f51eb97b05f483091c8105ebd560c5882680be8);
private:
    git_t() { }
CONST_SINGLETON(git_t);
}& git = git_t::instance();

