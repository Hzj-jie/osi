
#include "../../../app_info/error_handle.hpp"
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
                ERROR_POS);
    raise_error(error_type::exclamation,
                "this is some sample exclamation",
                ERROR_POS);
    raise_error("this is some sample informaiton",
                ERROR_POS);

    for(uint32_t i = uint32_t(error_type::first) + 1;
        i < uint32_t(error_type::last);
        i++)
    {
        ostringstream os;
        os << "this is some sample log " << i;
        raise_error(error_type(i),
                    os.str(),
                    ERROR_POS);
        raise_error(error_type(i),
                    static_cast<ostringstream&>(ostringstream() << "this is some sample log " << i),
                    ERROR_POS);
    }
}

