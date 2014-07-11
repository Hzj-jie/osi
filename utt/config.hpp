
#pragma once
#include <thread>
#include <stdint.h>
#include "../template/singleton.hpp"

const static class config_t
{
public:
    const uint32_t thread_count;

    bool selected(const std::string& name) const
    {
        return true;
    }

    bool selected(const icase& c) const
    {
        return selected(c.name());
    }
private:
    config_t() : thread_count(std::thread::hardware_concurrency()) { }
CONST_SINGLETON(config_t);
}& config = config_t::instance();

