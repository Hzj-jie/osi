
#pragma once
#include "../../utils/strcmp.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../app_info/assert.hpp"
#include "../../utils/strutils.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <limits.h>

class str_contains_test : public icase
{
private:
    template <typename Check, typename CheckStr>
    bool run_case(const char* const s, Check&& c, bool exp, CheckStr&& check_str)
    {
        utt_assert.equal(str_contains(std::string(s), c),
                         exp,
                         "std::string(\"",
                         s,
                         "\"), ",
                         check_str);
        utt_assert.equal(str_contains(s, c),
                         exp,
                         "\"",
                         s,
                         "\", ",
                         check_str);
        return true;
    }

    template <typename UnaryPredicate>
    bool run_case(const char* const s, UnaryPredicate&& c, bool exp)
    {
        return run_case(s, c, exp, "UnaryPredicate");
    }

    bool run_case(const char* const s, const char c, bool exp)
    {
        return run_case(s, c, exp, c);
    }

    bool run_case(const char* const s)
    {
        using namespace std;
        vector<bool> contains;
        contains.resize(CHAR_MAX - CHAR_MIN + 1, false);
        for(size_t i = 0; i < strlen(s); i++)
            contains[s[i] - CHAR_MIN] = true;
        for(size_t i = 0; i < strlen(s); i++)
        {
            if(!run_case(s, s[i], true)) return false;
            if(!run_case(s,
                         [&](const char c)
                         {
                             return c == s[i];
                         },
                         true)) return false;
        }
        for(int i = CHAR_MIN; i <= CHAR_MAX; i++)
        {
            if(!run_case(s, static_cast<char>(i), contains[i - CHAR_MIN])) return false;
            if(!run_case(s,
                         [&](const char c)
                         {
                             return c == static_cast<char>(i);
                         },
                         contains[i - CHAR_MIN])) return false;
        }
        return true;
    }
public:
    bool run() override
    {
        return run_case("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }

    DEFINE_CASE(str_contains_test);
};

REGISTER_CASE(str_contains_test);

