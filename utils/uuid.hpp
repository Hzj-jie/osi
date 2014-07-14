
#pragma once
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>

static std::string uuid_long_str()
{
    using namespace std;
    using namespace boost::uuids;
    return to_string(random_generator()());
}

static std::string uuid_short_str()
{
    using namespace std;
    string s = uuid_long_str();
    string o;
    for(size_t i = 0; i < s.size(); i++)
        if(s[i] != '-') o.push_back(s[i]);
    return o;
}

#define uuid_original_str() uuid_long_str()
#define uuid_str() uuid_short_str()

