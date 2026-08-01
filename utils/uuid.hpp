#pragma once
#include <string>
#include <chrono>
#include <atomic>
#include <random>
#include <sstream>
#include <iomanip>

static std::string uuid_long_str()
{
    static std::atomic<uint64_t> counter(0);
    static thread_local std::mt19937_64 rng(std::random_device{}());
    
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    uint64_t seq = ++counter;
    uint64_t r = rng();

    std::ostringstream ss;
    ss << std::hex << std::setfill('0')
       << std::setw(8) << static_cast<uint32_t>(now & 0xFFFFFFFFULL) << "-"
       << std::setw(4) << static_cast<uint16_t>((now >> 32) & 0xFFFFULL) << "-"
       << std::setw(4) << static_cast<uint16_t>((r >> 48) & 0xFFFFULL) << "-"
       << std::setw(4) << static_cast<uint16_t>((r >> 32) & 0xFFFFULL) << "-"
       << std::setw(12) << static_cast<uint64_t>((seq ^ r) & 0xFFFFFFFFFFFFULL);
    return ss.str();
}

static std::string uuid_short_str()
{
    std::string s = uuid_long_str();
    std::string o;
    for(char c : s)
        if(c != '-') o.push_back(c);
    return o;
}

#define uuid_original_str() uuid_long_str()
#define uuid_str() uuid_short_str()
