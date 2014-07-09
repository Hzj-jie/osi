
#include <stdint.h>

enum class error_type : uint32_t
{
    first,
    application,
    critical,
    exclamation,
    information,
    system,
    warning,
    user,
    performance,
    other,
    last
};

namespace error_handle
{
    const static char error_type_char[] = "_aceiswup__";

    static error_type char_to_error_type(char c)
    {
        for(uint32_t i = uint32_t(error_type::first) + 1;
            i < uint32_t(error_type::last);
            i++)
        {
            if(error_type_char[i] == c)
                return error_type(i);
        }
        return error_type::other;
    }

    static char error_type_to_char(error_type err_type, char err_type_char)
    {
        if(err_type > error_type::first &&
           err_type < error_type::last)
        {
            return error_type_char[uint32_t(err_type)];
        }
        else return err_type_char;
    }
}

