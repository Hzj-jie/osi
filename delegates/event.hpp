
#pragma once
#include <vector>
#include <functional>
#include <initializer_list>
#include <utility>
#include "../formation/movable_initializer_list.hpp"
#include <boost/predef.h>

template <typename... Args>
class event
{
private:
    std::vector<std::function<void(Args...)>> v;

public:
    event() = default;

#if !BOOST_COMP_MSVC
    event(std::function<void(Args...)>&& f)
    {
        v.push_back(std::forward<std::function<void(Args...)>>(f));
    }
#endif

    event(const std::movable_initializer_list<std::function<void(Args...)>>& s)
    {
        for(auto it = s.begin(); it != s.end(); it++)
            v.push_back(std::move(*it));
    }

    void bind(std::function<void(Args...)>&& f)
    {
        v.push_back(std::move(f));
    }

    template <typename T>
    event& operator+(T&& f)
    {
        bind(std::forward<T>(f));
        return *this;
    }

    template <typename T>
    event& operator+=(T&& f)
    {
        bind(std::forward<T>(f));
        return *this;
    }

    void execute(Args&&... args)
    {
        for(auto it = v.begin(); it != v.end(); it++)
            (*it)(std::forward<Args>(args)...);
    }
};

