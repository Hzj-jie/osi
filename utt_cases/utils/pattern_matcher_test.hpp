
#pragma once
#include "../../utils/str_pattern_matcher.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <vector>

class pattern_matcher_test : public icase
{
private:
    struct case_t
    {
        const char* const pattern;
        const char* const str;
        const int result;

        static case_t match(const char* const pattern,
                            const char* const str)
        {
            return case_t{pattern, str, case_sensitive_str_pattern_matcher::match};
        }

        static case_t unmatch(const char* const pattern,
                              const char* const str)
        {
            return case_t{pattern, str, case_sensitive_str_pattern_matcher::unmatch};
        }

        static case_t undetermined(const char* const pattern,
                                   const char* const str)
        {
            return case_t{pattern, str, case_sensitive_str_pattern_matcher::undetermined};
        }
    };

    bool match_one_case()
    {
        std::vector<case_t> match_one_cases =
        {
            // match
            case_t::match("", ""),
            case_t::match("*", "abc"),
            case_t::match("***", "abc"),
            case_t::match("a?c", "abc"),
            case_t::match("???", "abc"),
            case_t::match("*??", "abc"),
            case_t::match("*bc", "abc"),
            case_t::match("*b?", "abc"),
            case_t::match("*?*??***", "abc"),
            case_t::match("*???", "abc"),

            // undetermined
            case_t::undetermined("", "abc"),
            case_t::undetermined("abc", "bcd"),
            case_t::undetermined("a??", "ab"),
            case_t::undetermined("??c", "bc"),
            case_t::undetermined("???", "abcd"),
            case_t::undetermined("????", "abc"),
            case_t::undetermined("**??****??***", "abc"),
            case_t::undetermined("*?*?*?d", "bcd"),

            // unmatch
            case_t::unmatch("-abc", "abc"),
            case_t::unmatch("-*ab", "bbbbbab"),
            case_t::unmatch("-***", "lajdsljaldsjf")
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

