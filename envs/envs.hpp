
#pragma once
#include <map>
#include <string>
#include <stdlib.h>
#include "os.hpp"
#include <vector>
#include "../utils/strutils.hpp"
#include "../const/character.hpp"
#include "../template/singleton.hpp"

#if defined(OS_WINDOWS)
#include <windows.h>
namespace __envs_private
{
    static std::vector<std::string> getenvs()
    {
        std::vector<std::string> r;
        LPTCH envs = GetEnvironmentStrings();
        split(std::string(envs), r, character.null_s());
        FreeEnvironmentStrings(envs);
        return r;
    }
}
#elif defined(OS_POSIX)
namespace __envs_private
{
    static std::vector<std::string> getenvs()
    {
        std::vector<std::string> r;
        for(size_t i = 0; environ[i] != nullptr; i++)
            r.push_back(std::string(environ[i]));
        return r;
    }
}
#endif

const static class envs_t
{
private:
    std::map<std::string, std::string> e;

    envs_t()
    {
        std::vector<std::string>&& v = __envs_private::getenvs();
        for(size_t i = 0; i < v.size(); i++)
        {
            std::string k, s;
            split(v[i], k, s, character.equal_sign_s());
            e[k] = s;
        }
    }

public:
    const std::map<std::string, std::string>& operator()() const
    {
        return e;
    }

    const std::string& operator[](const std::string& key) const
    {
        static std::string empty;
        auto it = e.find(key);
        if(it == e.end()) return empty;
        else return (*it).second;
    }

    const std::string& operator[](const char* key) const
    {
        return operator[](std::string(key));
    }

    bool operator()(const std::string& key, std::string& r) const
    {
        auto it = e.find(key);
        if(it == e.end()) return false;
        else
        {
            r = (*it).second;
            return true;
        }
    }

    bool operator()(const char* const key, std::string& r) const
    {
        return operator()(std::string(key), r);
    }

    template <typename T>
    bool operator()(const T& keys, std::string& r) const
    {
        for(size_t i = 0; i < keys.size(); i++)
            if(operator()(keys[i], r)) return true;
        return false;
    }

    template <typename KEY_T, typename T>
    bool operator()(const KEY_T& key, T& r) const
    {
        std::string s;
        return operator()(key, s) &&
               from_str(s, r);
    }

    template <typename KEY_T, typename T>
    T get(const KEY_T& key) const
    {
        T r;
        return (operator()(key, r) ? r : T());
    }

    bool has(const std::string& key) const
    {
        return e.find(key) != e.end();
    }

    bool has(const char* key) const
    {
        return has(std::string(key));
    }

    template <typename T>
    bool has(const T& keys) const
    {
        for(size_t i = 0; i < keys.size(); i++)
            if(has(keys[i])) return true;
        return false;
    }

    CONST_SINGLETON(envs_t);
}& envs = envs_t::instance();

