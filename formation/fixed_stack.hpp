
#pragma once
#include <stdlib.h>
#include <array>
#include <utility>
#include <memory>
#include "../app_info/assert.hpp"

template <typename T, size_t MAX_SIZE>
class fixed_stack
{
private:
    std::array<T, MAX_SIZE> q;
    size_t index;

    bool index_is(size_t expected) const
    {
        assert(index >= 0 && index <= MAX_SIZE);
        return index == expected;
    }

public:
    bool empty() const
    {
        return index_is(0);
    }
    
    bool full() const
    {
        return index_is(MAX_SIZE);
    }

    T& back() const
    {
        assert(!empty());
        return q[index - 1];
    }

    bool pop(T& o)
    {
        if(empty()) return false;
        else
        {
            index--;
            new (&o) T(std::move(q[index]));
            return true;
        }
    }

    template <typename... Args>
    bool emplace(Args&&... args)
    {
        if(full()) return false;
        else
        {
            new (&q[index]) T(std::forward<Args>(args)...);
            index++;
            return true;
        }
    }

    template <typename U>
    bool push(U&& i)
    {
        return emplace(std::forward<U>(i));
    }

    void clear()
    {
        T o;
        while(pop(o));
    }
};

