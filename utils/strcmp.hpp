
#pragma once
#include <ctype.h>
#include <string>
#include <string.h>

namespace __strcmp_private
{
    static int case_insensitive_compare(const char* a, const char* b)
    {
        return strcasecmp(a, b);
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
            STRCMP_COMPARE_TEMPLATE(const char&, OP); \
            STRCMP_COMPARE_TEMPLATE(const char*, OP); \
            STRCMP_COMPARE_TEMPLATE(const std::string&, OP); };
STRCMP_STRUCT_TEMPLATE(equal, ==);
STRCMP_STRUCT_TEMPLATE(less, <);
#undef STRCMP_STRUCT_TEMPLATE
#undef STRCMP_COMPARE_TEMPLATE

