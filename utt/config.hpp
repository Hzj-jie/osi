
#pragma once
#include <thread>
#include <stdint.h>
#include <vector>
#include <set>
#include <string>
#include "../utils/str_pattern_matcher.hpp"
#include "../utils/strcmp.hpp"
#include "icase.hpp"

class config_t
{
private:
    std::vector<std::string> args;
    std::set<std::string, case_insensitive_less> sep;

public:
    const uint32_t thread_count;

    bool selected(const std::string& name) const
    {
        return args.empty() ||
               case_insensitive_str_pattern_matcher::match_any(args, name);
    }

    bool selected(const icase& c) const
    {
        return selected(c.name());
    }

    bool specific(const std::string& name) const
    {
        return sep.find(name) != sep.end();
    }

    bool specific(const icase& c) const
    {
        return specific(c.name());
    }

    config_t(const int argc, const char* const* const argv) :
        thread_count(std::thread::hardware_concurrency())
    {
        for(int i = 1; i < argc; i++)
        {
            args.push_back(std::string(argv[i]));
            sep.insert(std::string(argv[i]));
        }
    }

    config_t() : thread_count(0) { }
};

static config_t config;

