
#pragma once
#include <vector>
#include <stdlib.h>
#include <initializer_list>
#include <utility>

namespace std {
template <typename T>
class movable_initializer_list
{
public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef vector<T>::iterator iterator;
    typedef vector<T>::const_iterator const_iterator;
private:
    std::vector<T> v;

public:
    movable_initializer_list(initializer_list<T*> i)
    {
        for(auto it = i.begin(); it != i.end(); it++)
        {
            v.push_back(move(**it));
            delete *it;
        }
    }

    movable_initializer_list() = default;

    size_type size() const
    {
        return v.size();
    }

    iterator begin()
    {
        return v.begin();
    }

    const_iterator begin() const
    {
        return v.begin();
    }

    iterator end()
    {
        return v.end();
    }

    const_iterator end() const
    {
        return v.end();
    }
}; }

