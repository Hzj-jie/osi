
#pragma once
#include <stdint.h>
#include <vector>
#include <map>

template <typename T>
struct equal
{
    bool operator()(const T& a, const T& b) const
    {
        return a == b;
    }
};

template <typename CharT,
          CharT cut_sign,
          CharT add_sign,
          CharT multi_matching_sign,
          CharT single_matching_sign,
          typename Equal = equal<CharT> >
class pattern_matcher
{
public:
    const static int match = 1;
    const static int unmatch = 2;
    const static int undetermined = 0;

private:
    const static Equal equal;

    const static bool match_pattern(const CharT* const pattern,
                                    const uint32_t pattern_start,
                                    const uint32_t pattern_len,
                                    const CharT* const str,
                                    const uint32_t str_start,
                                    const uint32_t str_len)
    {
        uint32_t i = pattern_start;
        uint32_t j = str_start;
        while(i < pattern_len && j < str_len)
        {
            if(equal(pattern[i], multi_matching_sign))
            {
                i++;
                while(equal(pattern[i], multi_matching_sign)) i++;
                while(j <= str_len)
                {
                    if(match_pattern(pattern, i, pattern_len, str, j, str_len)) return true;
                    else j++;
                }
                return false;
            }
            else if(pattern[i] == single_matching_sign)
            {
                i++;
                j++;
            }
            else
            {
                if(equal(pattern[i], str[j]))
                {
                    i++;
                    j++;
                }
                else break;
            }
        }
        while(equal(pattern[i], multi_matching_sign)) i++;
        return (i == pattern_len && j == str_len);
    }

public:
    static int match_one(const CharT* const pattern,
                         const uint32_t pattern_start,
                         const uint32_t pattern_len,
                         const CharT* const str,
                         const uint32_t str_start,
                         const uint32_t str_len)
    {
        if(pattern_len > pattern_start &&
           equal(pattern[pattern_start], cut_sign))
        {
            return (match_pattern(pattern,
                                  pattern_start + 1,
                                  pattern_len,
                                  str,
                                  str_start,
                                  str_len) ?
                    unmatch :
                    undetermined);
        }
        else
        {
            uint32_t offset = ((pattern_len > pattern_start &&
                               equal(pattern[pattern_start], add_sign)) ?
                               1:
                               0);
            return (match_pattern(pattern,
                                  pattern_start + offset,
                                  pattern_len,
                                  str,
                                  str_start,
                                  str_len) ?
                    match :
                    undetermined);
        }
    }

    static bool match_any(const std::vector<std::pair<const CharT* const, uint32_t> >& patterns,
                          const CharT* const str,
                          const uint32_t str_start,
                          const uint32_t str_len)
    {
        bool matched = false;
        for(size_t i = 0; i < patterns.size(); i++)
        {
            int r = match_one(patterns[i].first,
                              0,
                              patterns[i].second,
                              str,
                              str_start,
                              str_len);
            if(r == match) matched = true;
            else if(r == unmatch) matched = false;
        }
        return matched;
    }
};

