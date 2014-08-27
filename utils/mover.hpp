
#pragma once
#include "convertor.hpp"
#include "../template/singleton.hpp"

const static class mover_t
{
private:
    mover_t() = default;

public:
    template <typename T>
    bool operator()(T& i, T& o) const
    {
        return convertor(std::move(i), o);
    }

    template <typename T>
    bool operator()(T&& i, T& o) const
    {
        return convertor(std::forward<T>(i), o);
    }

    template <typename T, typename... Args>
    bool operator()(Args&&... args, T& o) const
    {
        return convertor(T(std::forward<Args>(args)...), o);
    }

    CONST_SINGLETON(mover_t);
}& mover = mover_t::instance();

