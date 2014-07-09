
#include "../template/singleton.hpp"
#include <string>

const static class git_t
{
public:
    CONST_STATIC_STRING(commit, feeb25b0431509f52b175c9bb963d5ce7890d126);
private:
    git_t() { }
CONST_SINGLETON(git_t);
}& git = git_t::instance();

