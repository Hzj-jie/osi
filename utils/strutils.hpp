
#pragma once
#include <utility>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include "../app_info/k_assert.hpp"

static void to_upper(std::vector<std::string>& vs)
{
    for(auto& s : vs)
        for(char& c : s) c = std::toupper(static_cast<unsigned char>(c));
}

static void to_lower(std::vector<std::string>& vs)
{
    for(auto& s : vs)
        for(char& c : s) c = std::tolower(static_cast<unsigned char>(c));
}

static void to_initial_upper(std::vector<std::string>& vs)
{
    for(auto& s : vs)
    {
        if(!s.empty())
        {
            s[0] = std::toupper(static_cast<unsigned char>(s[0]));
            for(size_t i = 1; i < s.size(); ++i)
                s[i] = std::tolower(static_cast<unsigned char>(s[i]));
        }
    }
}

static void split(const std::string& s, std::vector<std::string>& o, const char* delims)
{
    o.clear();
    if(s.empty() || delims == nullptr) return;
    std::string token;
    for(char c : s)
    {
        if(std::strchr(delims, c) != nullptr)
        {
            if(!token.empty())
            {
                o.push_back(token);
                token.clear();
            }
        }
        else
        {
            token.push_back(c);
        }
    }
    if(!token.empty())
    {
        o.push_back(token);
    }
}

static void split(const std::string& s, std::vector<std::string>& o, const std::string& delims)
{
    split(s, o, delims.c_str());
}

static void split(const std::string& s, std::vector<std::string>& o)
{
    split(s, o, "\t,; ");
}

static bool split(const std::string& s, std::string& first, std::string& second, const char* delims)
{
    using namespace std;
    if(s.empty()) return false;
    else
    {
        size_t p = s.find_first_of(delims);
        if(p == string::npos)
        {
            first = s;
            second = string();
            return false;
        }
        else
        {
            first = s.substr(0, p);
            second = s.substr(p + 1);
            return true;
        }
    }
}

static bool split(const std::string& s, std::string& first, std::string& second, const std::string& delims)
{
    return split(s, first, second, delims.c_str());
}

static bool split(const std::string& s, std::string& first, std::string& second)
{
    return split(s, first, second, " \t");
}

template <typename T>
static bool from_str(const std::string& s, T& o)
{
    using namespace std;
    istringstream convert(s);
    return static_cast<bool>(convert >> o);
}

template <typename T>
static bool to_str(const T& i, std::string& o)
{
    using namespace std;
    ostringstream convert;
    if(convert << i)
    {
        o = convert.str();
        return true;
    }
    else return false;
}

static bool to_str(const std::string& s, std::string& o)
{
    o = s;
    return true;
}

static bool to_str(const char& i, std::string& o)
{
    o.clear();
    o.push_back(i);
    return true;
}

static std::string to_str(const std::string& i)
{
    std::string o;
    k_assert(to_str(i, o));
    return o;
}

static std::string to_str(const char& i)
{
    std::string o;
    k_assert(to_str(i, o));
    return o;
}

namespace __strutils_private
{
    void append_to_stream(std::ostream&) { }

    template <typename T, typename... Args>
    void append_to_stream(std::ostream& s, T&& f, Args&&... others)
    {
        s << f;
        append_to_stream(s, others...);
    }
}

template <typename... Args>
std::string strcat(Args&&... args)
{
    using namespace std;
    using namespace __strutils_private;
    ostringstream os;
    append_to_stream(os, std::forward<Args>(args)...);
    return os.str();
}

