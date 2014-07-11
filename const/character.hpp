
#pragma once
#include <string>
#include "../template/singleton.hpp"

const static class character_t
{
public:
#define DEFINE_CHARACTER(name, x) \
    const static char name = x; \
    CONST_STATIC_STRING_EXP(name##_s, std::string(1, x));

    DEFINE_CHARACTER(null, '\0');
    DEFINE_CHARACTER(blank, ' ');
    DEFINE_CHARACTER(minus_sign, '-');
    DEFINE_CHARACTER(colon, ':');
    DEFINE_CHARACTER(underscore, '_');
    DEFINE_CHARACTER(at, '@');
    DEFINE_CHARACTER(comma, ',');
    DEFINE_CHARACTER(dot, '.');
    DEFINE_CHARACTER(u, 'u');

private:
    character_t() { }
#undef DEFINE_CHARACTER
    CONST_SINGLETON(character_t);
}& character = character_t::instance();
