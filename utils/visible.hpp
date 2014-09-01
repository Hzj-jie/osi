
#pragma once
#include <iostream>
#include <utility>

template <typename T>
struct visible
{
private:
    const T x;

public:
    template <typename U>
    visible(U&& x) :
        x(std::forward<U>(x)) { }
};

