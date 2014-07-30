
#pragma once
#include "../../utils/str_pattern_matcher.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <vector>
#include <boost/predef.h>

class pattern_matcher_test : public icase
{
private:
    struct match_one_case_t
    {
#if BOOST_COMP_MSVC
        char* pattern;
        char* str;
        int result;

        match_one_case_t& operator=(const match_one_case_t&) = default;
        match_one_case_t() = default;
        match_one_case_t(char* pattern,
                         char* str,
                         int result) :
            pattern(pattern),
            str(str),
            result(result) { }
#else
        const char* const pattern;
        const char* const str;
        const int result;
#endif

#if BOOST_COMP_MSVC
        static match_one_case_t match(char* pattern,
                                      char* str)
        {
            return match_one_case_t(pattern, str, case_sensitive_str_pattern_matcher::match);
        }
#else
        static match_one_case_t match(const char* const pattern,
                                      const char* const str)
        {
            return match_one_case_t{pattern, str, case_sensitive_str_pattern_matcher::match};
        }
#endif

#if BOOST_COMP_MSVC
        static match_one_case_t unmatch(char* pattern,
                                        char* str)
        {
            return match_one_case_t(pattern, str, case_sensitive_str_pattern_matcher::unmatch);
        }
#else
        static match_one_case_t unmatch(const char* const pattern,
                                        const char* const str)
        {
            return match_one_case_t{pattern, str, case_sensitive_str_pattern_matcher::unmatch};
        }
#endif

#if BOOST_COMP_MSVC
        static match_one_case_t undetermined(char* pattern,
                                             char* str)
        {
            return match_one_case_t(pattern, str, case_sensitive_str_pattern_matcher::undetermined);
        }
#else
        static match_one_case_t undetermined(const char* const pattern,
                                             const char* const str)
        {
            return match_one_case_t{pattern, str, case_sensitive_str_pattern_matcher::undetermined};
        }
#endif
    };

    bool match_one_case()
    {
        std::vector<match_one_case_t> match_one_cases =
        {
            // match
            match_one_case_t::match("", ""),
            match_one_case_t::match("*", "abc"),
            match_one_case_t::match("***", "abc"),
            match_one_case_t::match("a?c", "abc"),
            match_one_case_t::match("???", "abc"),
            match_one_case_t::match("*??", "abc"),
            match_one_case_t::match("*bc", "abc"),
            match_one_case_t::match("*b?", "abc"),
            match_one_case_t::match("*?*??***", "abc"),
            match_one_case_t::match("*???", "abc"),

            // undetermined
            match_one_case_t::undetermined("", "abc"),
            match_one_case_t::undetermined("abc", "bcd"),
            match_one_case_t::undetermined("a??", "ab"),
            match_one_case_t::undetermined("??c", "bc"),
            match_one_case_t::undetermined("???", "abcd"),
            match_one_case_t::undetermined("????", "abc"),
            match_one_case_t::undetermined("**??****??***", "abc"),
            match_one_case_t::undetermined("*?*?*?d", "bcd"),

            // unmatch
            match_one_case_t::unmatch("-abc", "abc"),
            match_one_case_t::unmatch("-*ab", "bbbbbab"),
            match_one_case_t::unmatch("-***", "lajdsljaldsjf")
        };

        for(size_t i = 0; i < match_one_cases.size(); i++)
        {
            int result = case_sensitive_str_pattern_matcher::
                            match_one(match_one_cases[i].pattern,
                                      match_one_cases[i].str);
            utt_assert.equal(result,
                             match_one_cases[i].result,
                             " pattern == ",
                             match_one_cases[i].pattern,
                             ", str == ",
                             match_one_cases[i].str,
                             ", expected == ",
                             match_one_cases[i].result,
                             ", real == ",
                             result);
        }
        return true;
    }

    bool match_any_case()
    {
        return true;
    }

public:
    bool run() override
    {
        return match_one_case() &&
               match_any_case();
    }

    DEFINE_CASE(pattern_matcher_test);
};

REGISTER_CASE(pattern_matcher_test);

