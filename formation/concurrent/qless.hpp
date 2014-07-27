
#pragma once
#include "slimqless.hpp"
#include <atomic>
#include <utility>
#include <stdlib.h>
#include "../../app_info/assert.hpp"

template <typename T>
class qless final
{
private:
    slimqless<T> q;
    std::atomic<int> s;

public:
    qless() :
        q(),
        s(0) { }

    void push(const T& v)
    {
        q.push(v);
        s++;
    }

    void push(T&& v)
    {
        q.push(std::forward<T>(v));
        s++;
    }

    template <typename... Args>
    void emplace_push(Args&&... args)
    {
        q.push(std::forward<Args>(args)...);
        s++;
    }

    bool pop(T& v)
    {
        if(s.fetch_sub(1) >= 1)
            return assert(q.pop(v));
        else return false;
    }

    size_t size() const
    {
        int ss = s;
        return (ss < 0 ? 0 : ss);
    }

    void clear()
    {
        T v;
        while(pop(v));
    }
};

