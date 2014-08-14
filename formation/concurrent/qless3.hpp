
#pragma once
#include <queue>
#include <stdlib.h>
#include <mutex>
#include <utility>
#include "../../sync/lock.hpp"

template <typename T>
class qless3 final
{
private:
    std::queue<T> q;
    mutable std::mutex mtx;

public:
    void push(const T& v)
    {
        scope_lock(mtx);
        q.push(v);
    }

    void push(T&& v)
    {
        scope_lock(mtx);
        q.push(std::forward<T>(v));
    }

    template <typename... Args>
    void emplace_push(Args&&... args)
    {
        scope_lock(mtx);
        q.emplace(std::forward<Args>(args)...);
    }

    bool pop(T& v)
    {
        scope_lock(mtx);
        if(q.empty()) return false;
        else
        {
            new (&v) T(std::move(q.front()));
            q.pop();
            return true;
        }
    }

    size_t size() const
    {
        scope_lock(mtx);
        return q.size();
    }

    bool empty() const
    {
        scope_lock(mtx);
        return q.empty();
    }

    void clear()
    {
        scope_lock(mtx);
        while(!q.empty()) q.pop();
    }
};

