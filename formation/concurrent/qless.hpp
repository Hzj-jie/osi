
#pragma once
#include "slimqless.hpp"
#include "slimqless2.hpp"
#include <atomic>
#include <utility>
#include <stdlib.h>
#include <boost/predef.h>
#include "../../app_info/assert.hpp"

template <template <typename T> class Slimqless, typename T>
class qless_template final
{
private:
    Slimqless<T> q;
    std::atomic<int> s;

public:
    qless_template() :
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
        // s.fetch_add(1, std::memory_order_release);
        s++;
    }

    template <typename... Args>
    void emplace(Args&&... args)
    {
        q.emplace(std::forward<Args>(args)...);
        // s.fetch_add(1, std::memory_order_release);
        s++;
    }

    bool pop(T& v)
    {
        // if(s.fetch_sub(1, std::memory_order_release) >= 1)
        if((s--) >= 1)
            return assert(q.pop(v));
        else
        {
            // s.fetch_add(1, std::memory_order_release);
            s++;
            return false;
        }
    }

    size_t size() const
    {
        // int ss = s.load(std::memory_order_acquire);
        int ss = s.load();
        return (ss < 0 ? 0 : ss);
    }

    bool empty() const
    {
        return size() == 0;
    }

    void clear()
    {
        T v;
        while(pop(v));
    }
};

template <typename T>
using qless = qless_template<slimqless, T>;

template <typename T>
using qless2 = qless_template<slimqless2, T>;

