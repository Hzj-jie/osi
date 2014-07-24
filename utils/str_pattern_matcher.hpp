
#pragma once
#include "pattern_matcher.hpp"
#include <string.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <ctype.h>

template <typename Equal = equal<char> >
struct str_pattern_matcher_typedef
{
public:
    typedef pattern_matcher<char,
                            '-',
                            '+',
                            '*',
                            '?',
                            Equal>
            Type;
};

template <typename Equal = equal<char> >
class str_pattern_matcher :
        public str_pattern_matcher_typedef<Equal>::Type
{
private:
    typedef typename str_pattern_matcher_typedef<Equal>::Type Base;

    static std::vector<std::pair<const char*, uint32_t> >
           convert(const std::vector<const char*>& v)
    {
        using namespace std;
        vector<pair<const char*, uint32_t> > r;
        for(size_t i = 0; i < v.size(); i++)
            r.push_back(make_pair(v[i], strlen(v[i])));
        return r;
    }

    static std::vector<std::pair<const char*, uint32_t> >
           convert(const std::vector<std::string>& v)
    {
        using namespace std;
        vector<pair<const char*, uint32_t> > r;
        for(size_t i = 0; i < v.size(); i++)
            r.push_back(make_pair(v[i].c_str(), v[i].length()));
        return r;
    }

    static std::vector<std::pair<const char*, uint32_t> >
           convert(const std::vector<std::pair<const std::string, uint32_t> >& v)
    {
        using namespace std;
        vector<pair<const char*, uint32_t> > r;
        for(size_t i = 0; i < v.size(); i++)
            r.push_back(make_pair(v[i].first.c_str(), v[i].second));
        return r;
    }

public:
    static int match_one(const char* const pattern,
                         const uint32_t pattern_start,
                         const uint32_t pattern_len,
                         const char* const str,
                         const uint32_t str_start)
    {
        return Base::match_one(pattern,
                               pattern_start,
                               pattern_len,
                               str,
                               str_start,
                               strlen(str));
    }

    static int match_one(const char* const pattern,
                         const uint32_t pattern_start,
                         const char* const str,
                         const uint32_t str_start,
                         const uint32_t str_len)
    {
        return Base::match_one(pattern,
                               pattern_start,
                               strlen(pattern),
                               str,
                               str_start,
                               str_len);
    }

    static int match_one(const char* const pattern,
                         const uint32_t pattern_start,
                         const char* const str,
                         const uint32_t str_start)
    {
        return Base::match_one(pattern,
                               pattern_start,
                               strlen(pattern),
                               str,
                               str_start,
                               strlen(str));
    }

    static int match_one(const char* const pattern,
                         const uint32_t pattern_start,
                         const char* const str)
    {
        return Base::match_one(pattern,
                               pattern_start,
                               strlen(pattern),
                               str,
                               0,
                               strlen(str));
    }

    static int match_one(const char* const pattern,
                         const char* const str,
                         const uint32_t str_start)
    {
        return Base::match_one(pattern,
                               0,
                               strlen(pattern),
                               str,
                               str_start,
                               strlen(str));
    }

    static int match_one(const char* const pattern,
                         const char* const str)
    {
        return Base::match_one(pattern,
                               0,
                               strlen(pattern),
                               str,
                               0,
                               strlen(str));
    }

    static int match_one(const std::string& pattern,
                         const uint32_t pattern_start,
                         const uint32_t pattern_len,
                         const std::string& str,
                         const uint32_t str_start,
                         const uint32_t str_len)
    {
        return Base::match_one(pattern.c_str(),
                               pattern_start,
                               pattern_len,
                               str.c_str(),
                               str_start,
                               str_len);
    }

    static int match_one(const std::string& pattern,
                         const uint32_t pattern_start,
                         const uint32_t pattern_len,
                         const std::string& str,
                         const uint32_t str_start)
    {
        return Base::match_one(pattern.c_str(),
                               pattern_start,
                               pattern_len,
                               str.c_str(),
                               str_start,
                               str.length());
    }

    static int match_one(const std::string& pattern,
                         const uint32_t pattern_start,
                         const std::string& str,
                         const uint32_t str_start,
                         const uint32_t str_len)
    {
        return Base::match_one(pattern.c_str(),
                               pattern_start,
                               pattern.length(),
                               str.c_str(),
                               str_start,
                               str_len);
    }

    static int match_one(const std::string& pattern,
                         const uint32_t pattern_start,
                         const std::string& str,
                         const uint32_t str_start)
    {
        return Base::match_one(pattern.c_str(),
                               pattern_start,
                               pattern.length(),
                               str.c_str(),
                               str_start,
                               str.length());
    }

    static int match_one(const std::string& pattern,
                         const uint32_t pattern_start,
                         const std::string& str)
    {
        return Base::match_one(pattern.c_str(),
                               pattern_start,
                               pattern.length(),
                               str.c_str(),
                               0,
                               str.length());
    }

    static int match_one(const std::string& pattern,
                         const std::string& str,
                         const uint32_t str_start)
    {
        return Base::match_one(pattern.c_str(),
                               0,
                               pattern.length(),
                               str.c_str(),
                               str_start,
                               str.length());
    }

    static int match_one(const std::string& pattern,
                         const std::string& str)
    {
        return Base::match_one(pattern.c_str(),
                               0,
                               pattern.length(),
                               str.c_str(),
                               0,
                               str.length());
    }

    static bool match_any(const std::vector<std::pair<const char*, uint32_t> >& patterns,
                          const char* const str,
                          const uint32_t str_start)
    {
        return Base::match_any(patterns,
                               str,
                               str_start,
                               strlen(str));
    }

    static bool match_any(const std::vector<std::pair<const char*, uint32_t> >& patterns,
                          const char* const str)
    {
        return Base::match_any(patterns,
                               str,
                               0,
                               strlen(str));
    }

    static bool match_any(const std::vector<std::pair<const char*, uint32_t> >& patterns,
                          const std::string& str,
                          const uint32_t str_start,
                          const uint32_t str_len)
    {
        return Base::match_any(patterns,
                               str.c_str(),
                               str_start,
                               str_len);
    }

    static bool match_any(const std::vector<std::pair<const char*, uint32_t> >& patterns,
                          const std::string& str,
                          const uint32_t str_start)
    {
        return Base::match_any(patterns,
                               str.c_str(),
                               str_start,
                               str.length());
    }

    static bool match_any(const std::vector<std::pair<const char*, uint32_t> >& patterns,
                          const std::string& str)
    {
        return Base::match_any(patterns,
                               str.c_str(),
                               0,
                               str.length());
    }

    static bool match_any(const std::vector<const char*>& patterns,
                          const char* const str,
                          const uint32_t str_start,
                          const uint32_t str_len)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               str_len);
    }

    static bool match_any(const std::vector<const char*>& patterns,
                          const char* const str,
                          const uint32_t str_start)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               strlen(str));
    }

    static bool match_any(const std::vector<const char*>& patterns,
                          const char* const str)
    {
        return Base::match_any(convert(patterns),
                               str,
                               0,
                               strlen(str));
    }

    static bool match_any(const std::vector<std::string>& patterns,
                          const char* const str,
                          const uint32_t str_start,
                          const uint32_t str_len)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               str_len);
    }

    static bool match_any(const std::vector<std::string>& patterns,
                          const char* const str,
                          const uint32_t str_start)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               strlen(str));
    }

    static bool match_any(const std::vector<std::string>& patterns,
                          const char* const str)
    {
        return Base::match_any(convert(patterns),
                               str,
                               0,
                               strlen(str));
    }

    static bool match_any(const std::vector<const char*>& patterns,
                          const std::string& str,
                          const uint32_t str_start,
                          const uint32_t str_len)
    {
        return Base::match_any(convert(patterns),
                               str.c_str(),
                               str_start,
                               str_len);
    }

    static bool match_any(const std::vector<const char*>& patterns,
                          const std::string& str,
                          const uint32_t str_start)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               str.length());
    }

    static bool match_any(const std::vector<const char*>& patterns,
                          const std::string& str)
    {
        return Base::match_any(convert(patterns),
                               str,
                               0,
                               str.length());
    }

    static bool match_any(const std::vector<std::string>& patterns,
                          const std::string& str,
                          const uint32_t str_start,
                          const uint32_t str_len)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               str_len);
    }

    static bool match_any(const std::vector<std::string>& patterns,
                          const std::string& str,
                          const uint32_t str_start)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               str.length());
    }

    static bool match_any(const std::vector<std::string>& patterns,
                          const std::string& str)
    {
        return Base::match_any(convert(patterns),
                               str.c_str(),
                               0,
                               str.length());
    }

    static bool match_any(const std::vector<std::pair<const std::string, uint32_t> >& patterns,
                          const char* const str,
                          const uint32_t str_start,
                          const uint32_t str_len)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               str_len);
    }

    static bool match_any(const std::vector<std::pair<const std::string, uint32_t> >& patterns,
                          const char* const str,
                          const uint32_t str_start)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               strlen(str));
    }

    static bool match_any(const std::vector<std::pair<const std::string, uint32_t> >& patterns,
                          const char* const str)
    {
        return Base::match_any(convert(patterns),
                               str,
                               0,
                               strlen(str));
    }

    static bool match_any(const std::vector<std::pair<const std::string, uint32_t> >& patterns,
                          const std::string& str,
                          const uint32_t str_start,
                          const uint32_t str_len)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               str_len);
    }

    static bool match_any(const std::vector<std::pair<const std::string, uint32_t> >& patterns,
                          const std::string& str,
                          const uint32_t str_start)
    {
        return Base::match_any(convert(patterns),
                               str,
                               str_start,
                               str.length());
    }

    static bool match_any(const std::vector<std::pair<const std::string, uint32_t> >& patterns,
                          const std::string& str)
    {
        return Base::match_any(convert(patterns),
                               str,
                               0,
                               str.length());
    }
};

class case_sensitive_str_pattern_matcher :
        public str_pattern_matcher<equal<char> > { };

struct case_insensitive_equal
{
    bool operator()(const char& a, const char& b) const
    {
        return tolower(a) == tolower(b);
    }
};

class case_insensitive_str_pattern_matcher :
        public str_pattern_matcher<case_insensitive_equal> { };

