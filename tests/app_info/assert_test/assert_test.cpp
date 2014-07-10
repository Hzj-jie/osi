
#include "../../../app_info/error_handle.hpp"
#include "../../../app_info/assert.hpp"
#include "../../../utils/strutils.hpp"
using namespace std;

int main(int argc, const char* const* const argv)
{
    if(argc > 1)
        error_handle::enable_default_file_error_writer();
    if(!assert(true, strcat("this is a false assert")))
        raise_error(error_type::critical, "a false assert triggered");
    if(assert(false, strcat("this is a true assert")))
        raise_error(error_type::critical, "a true assert not triggered");
}

