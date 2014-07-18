
#pragma once
#include "../app_info/error_type.hpp"
#include "../app_info/error_handle.hpp"
#include "../const/character.hpp"
#include <utility>

namespace utt
{
    namespace
    {
        const static error_type err_type = error_type::other;
        const static char err_type_char = character.t;
    }

    template <typename... Args>
    static void utt_raise_error(Args&&... args)
    {
        raise_error(err_type, err_type_char, std::forward<Args>(args)...);
    }
}

