
#pragma once
#include "../template/singleton.hpp"

struct ternary
{
private:
    const int __unknown = 0;
    const int __true = 1;
    const int __false = 2;
    int d;

public:
    ternary(const ternary& i)
    {
        operator=(i);
    }

    ternary(bool i)
    {
        operator=(i);
    }

    ternary() : d(__unknown) { }

    ternary& operator=(const ternary& i)
    {
        d = i.d;
        return *this;
    }

    ternary& operator=(bool i)
    {
        d = (i ? __true : __false);
        return *this;
    }

    bool operator==(const ternary& i) const
    {
        return d == i.d;
    }

    bool operator!=(const ternary& i) const
    {
        return d != i.d;
    }

    bool true_() const
    {
        return d == __true;
    }

    bool false_() const
    {
        return d == __false;
    }

    bool unknown_() const
    {
        return d == __unknown;
    }

    bool nottrue() const
    {
        return !true_();
    }

    bool notfalse() const
    {
        return !false_();
    }

    bool notunknown() const
    {
        return !unknown_();
    }

    CONST_SINGLETON_FUNC(ternary, _unknown, { });
    CONST_SINGLETON_FUNC(ternary, _true, {true});
    CONST_SINGLETON_FUNC(ternary, _false, {false});
};

