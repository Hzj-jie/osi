#pragma once

#include <queue>
#include <mutex>
#include <utility>
#include <cstddef>

template <typename T>
class slimqless2
{
private:
    std::queue<T> q;
    mutable std::mutex mtx;

public:
    slimqless2() = default;
    explicit slimqless2(std::size_t /* capacity */) {}

    slimqless2(const slimqless2& other)
    {
        std::lock_guard<std::mutex> lock(other.mtx);
        q = other.q;
    }

    slimqless2& operator=(const slimqless2& other)
    {
        if (this != &other)
        {
            std::lock_guard<std::mutex> lock1(mtx);
            std::lock_guard<std::mutex> lock2(other.mtx);
            q = other.q;
        }
        return *this;
    }

    bool push(const T& val)
    {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(val);
        return true;
    }

    bool push(T&& val)
    {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(std::move(val));
        return true;
    }

    template <typename... Args>
    bool emplace(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(mtx);
        q.emplace(std::forward<Args>(args)...);
        return true;
    }

    bool pop(T& val)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (q.empty())
            return false;
        val = std::move(q.front());
        q.pop();
        return true;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }

    std::size_t size() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return q.size();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::queue<T> empty_q;
        std::swap(q, empty_q);
    }
};
