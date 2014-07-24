
#pragma once
#include <thread>
#include <stdint.h>
#include <vector>
#include <string>
#include "../utils/str_pattern_matcher.hpp"

class config_t
{
private:
    std::vector<std::string> args;

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

    config_t(const int argc, const char* const* const argv) :
        thread_count(std::thread::hardware_concurrency())
    {
        for(int i = 1; i < argc; i++)
            args.push_back(std::string(argv[i]));
    }

    config_t() : thread_count(0) { }
};

