
#pragma once
#include "envs.hpp"
#include "../template/singleton.hpp"
#include <vector>
#include <string>
#include <initializer_list>
#include <cctype>
#include "../utils/strutils.hpp"
#include <stdint.h>

const static class preenv_t
{
public:
    const bool busy_wait;
    const uint32_t queue_runner_thread_count;
    const uint32_t threadpool_thread_count;
    const uint32_t queue_runner_interval_ms;
    const uint32_t interval_ms;
    const uint32_t threadpool_stop_wait_ms;
private:
    static std::string join(const std::vector<std::string>& vec, const std::string& sep)
    {
        std::string res;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i > 0) res += sep;
            res += vec[i];
        }
        return res;
    }

    static void to_upper(std::vector<std::string>& vec)
    {
        for (auto& s : vec)
            for (char& c : s) c = std::toupper(static_cast<unsigned char>(c));
    }

    static void to_lower(std::vector<std::string>& vec)
    {
        for (auto& s : vec)
            for (char& c : s) c = std::tolower(static_cast<unsigned char>(c));
    }

    static void to_initial_upper(std::vector<std::string>& vec)
    {
        for (auto& s : vec)
        {
            if (!s.empty())
            {
                s[0] = std::toupper(static_cast<unsigned char>(s[0]));
                for (size_t i = 1; i < s.size(); ++i)
                    s[i] = std::tolower(static_cast<unsigned char>(s[i]));
            }
        }
    }

    static void add_combination(std::vector<std::string>& r, const std::vector<std::string>& inputs)
    {
        r.push_back(join(inputs, ""));
        r.push_back(join(inputs, "-"));
        r.push_back(join(inputs, "_"));
    }

    static std::vector<std::string> env_keys(const std::initializer_list<const char*>& segs)
    {
        std::vector<std::string> r;
        std::vector<std::string> inputs(segs.begin(), segs.end());
        add_combination(r, inputs);
        to_upper(inputs);
        add_combination(r, inputs);
        to_lower(inputs);
        add_combination(r, inputs);
        to_initial_upper(inputs);
        add_combination(r, inputs);
        return r;
    }

    template <typename T>
    T from_str(const std::string& s)
    {
        T o;
        if(::from_str(s, o)) return o;
        else return T();
    }

    std::string string_envs(const std::vector<std::string>& keys)
    {
        return ::envs.get<std::vector<std::string>, std::string>(keys);
    }

    preenv_t() :
        busy_wait(envs.has(env_keys({"busy", "wait"}))),
        queue_runner_thread_count(from_str<uint32_t>(string_envs(env_keys({"queue", "runner", "thread", "count"})))),
        threadpool_thread_count(from_str<uint32_t>(string_envs(env_keys({"threadpool", "thread", "count"})))),
        queue_runner_interval_ms(from_str<uint32_t>(string_envs(env_keys({"queue", "runner", "interval", "ms"})))),
        interval_ms(from_str<uint32_t>(string_envs(env_keys({"interval", "ms"})))),
        threadpool_stop_wait_ms(from_str<uint32_t>(string_envs(env_keys({"threadpool", "stop", "wait", "ms"}))))
    { }

    CONST_SINGLETON(preenv_t);
}& preenv = preenv_t::instance();

