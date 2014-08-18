
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
    DEFINE_CHARACTER(add_sign, '+');
    DEFINE_CHARACTER(asterisk, '*');
    DEFINE_CHARACTER(question_mark, '?');
    DEFINE_CHARACTER(colon, ':');
    DEFINE_CHARACTER(equal_sign, '=');
    DEFINE_CHARACTER(underscore, '_');
    DEFINE_CHARACTER(at, '@');
    DEFINE_CHARACTER(comma, ',');
    DEFINE_CHARACTER(dot, '.');
    DEFINE_CHARACTER(u, 'u');
    DEFINE_CHARACTER(t, 't');
#undef DEFINE_CHARACTER

private:
    character_t() = default;
    CONST_SINGLETON(character_t);
}& character = character_t::instance();

