
#pragma once
#include <string>
#include "../template/singleton.hpp"

const static class character_t
{
public:
#define DEFINE_CHARACTER(name, x) \
    const static char name = x[0]; \
    CONST_STATIC_STRING_EXP(name##_s, x);

    DEFINE_CHARACTER(blank, " ");
    DEFINE_CHARACTER(minus_sign, "-");
    DEFINE_CHARACTER(colon, ":");

private:
    character_t() { }
#undef DEFINE_CHARACTER
    CONST_SINGLETON(character_t);
}& character = character_t::instance();
