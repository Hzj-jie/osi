
#pragma once
#include "../app_info/error_type.hpp"
#include "../app_info/error_handle.hpp"
#include "../app_info/trace.hpp"
#include "../const/character.hpp"

namespace utt
{
    namespace
    {
        const static error_type err_type = error_type::other;
        const static char err_type_char = character.t;
    }
    template <typename T>
    static void raise_error(T&& err_msg)
    {
        raise_error(err_type, err_type_char, err_msg);
    }

    template <typename T>
    static void raise_error(T&& err_msg, const code_position& cp)
    {
        raise_error(err_type, err_type_char, err_msg, cp);
    }
}

template <typename T>
static void utt_raise_error(T&& err_msg)
{
    utt::raise_error(err_msg);
}

template <typename T>
static void utt_raise_error(T&& err_msg, const code_position& cp)
{
    utt::raise_error(err_msg, cp);
}

