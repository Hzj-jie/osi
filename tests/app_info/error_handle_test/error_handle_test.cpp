
#include "../../../app_info/error_handle.hpp"
#include "../../../utils/strutils.hpp"
#include <stdint.h>
#include <sstream>
using namespace std;

int main(int argc, const char* const* const argv)
{
    if(argc > 1)
        error_handle::enable_default_file_error_writer();
    raise_error(error_type::warning,
                'x',
                "this is some sample warning",
                CODE_POSITION());
    raise_error(error_type::exclamation,
                "this is some sample exclamation",
                CODE_POSITION());
    raise_error("this is some sample informaiton",
                CODE_POSITION());

    for(uint32_t i = uint32_t(error_type::first) + 1;
        i < uint32_t(error_type::last);
        i++)
    {
        raise_error(error_type(i),
                    strcat("this is some sample log ", i),
                    CODE_POS);
    }
}

