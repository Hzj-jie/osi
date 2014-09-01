
#pragma once
#include <vector>
#include <functional>
#include <initializer_list>
#include <utility>

template <typename... Args>
class event
{
private:
    std::vector<std::function<void(Args...)>> v;

public:
    event() = default;
    event(const std::initializer_list<std::function<void(Args...)>>& s)
    {
    }
};

