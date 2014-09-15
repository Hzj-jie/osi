
#pragma once
#include <vector>
#include <stdlib.h>
#include <initializer_list>
#include <utility>
#include <boost/predef.h>

namespace std {
template <typename T>
class movable_initializer_list
{
public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
private:
    std::vector<T> v;

public:
#if BOOST_COMP_MSVC
    movable_initializer_list(initializer_list<T*> i)
#else
    movable_initializer_list(const initializer_list<T*>& i)
#endif
    {
        for(auto it = i.begin(); it != i.end(); it++)
        {
            assert((*it) != nullptr);
            v.push_back(move(**it));
            delete *it;
        }
    }

#if BOOST_COMP_MSVC
    movable_initializer_list(initializer_list<T> i)
#else
    movable_initializer_list(const initializer_list<T>& i)
#endif
    {
        for(auto it = i.begin(); it != i.end(); it++)
            v.push_back(move(*it));
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

