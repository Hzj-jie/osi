
#pragma once
#include <ctype.h>
#include <string>
#include <string.h>
#include "../envs/os.hpp"
#include <algorithm>

namespace __strcmp_private
{
    static int case_insensitive_compare(const char* a, const char* b)
    {
#if defined (OS_POSIX)
        return strcasecmp(a, b);
#elif defined (OS_WINDOWS)
        return stricmp(a, b);
#endif
    }

    static int case_insensitive_compare(const char& a, const char& b)
    {
        return tolower(a) - tolower(b);
    }

    static int case_insensitive_compare(const std::string& a, const std::string& b)
    {
        return case_insensitive_compare(a.c_str(), b.c_str());
    }
}

#define STRCMP_COMPARE_TEMPLATE(type, OP) \
            bool operator()(type a, type b) const { \
                return __strcmp_private::case_insensitive_compare(a, b) OP 0; }
#define STRCMP_STRUCT_TEMPLATE(name, OP) \
        struct case_insensitive_##name { \
            case_insensitive_##name() { } \
            STRCMP_COMPARE_TEMPLATE(const char&, OP); \
            STRCMP_COMPARE_TEMPLATE(const char*, OP); \
            STRCMP_COMPARE_TEMPLATE(const std::string&, OP); };
STRCMP_STRUCT_TEMPLATE(equal, ==);
STRCMP_STRUCT_TEMPLATE(less, <);
#undef STRCMP_STRUCT_TEMPLATE
#undef STRCMP_COMPARE_TEMPLATE

static int isnull(int c)
{
    return c;
}

template <typename UnaryPredicate>
static bool str_contains(const char* s, UnaryPredicate&& pred)
{
    return std::any_of(s, s + strlen(s), pred);
}

template <typename UnaryPredicate>
static bool str_contains(const std::string& s, UnaryPredicate&& pred)
{
    return std::any_of(s.begin(), s.end(), pred);
}

static bool str_contains(const char* s, const char c)
{
    return str_contains(s,
                        [&](const char cur)
                        {
                            return cur == c;
                        });
}

static bool str_contains(const std::string& s, const char c)
{
    return str_contains(s,
                        [&](const char cur)
                        {
                            return cur == c;
                        });
}

