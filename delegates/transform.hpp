
#pragma once
#include <functional>
#include <utility>

template <typename... Args>
static std::function<bool(Args...)> reverse(const std::function<bool(Args...)>& f)
{
    return [&](Args... args) { return !f(std::forward<Args>(args)...); };
}

