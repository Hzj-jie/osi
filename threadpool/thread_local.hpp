
#pragma once
#include <unordered_map>
#include <thread>

template <typename T>
class thread_local_storage
{
private:
    std::unordered_map<std::thread::id, T> s;

public:
    T& operator*()
    {
        return s[std::this_thread::get_id()];
    }

    const T& operator*() const
    {
        return s[std::this_thread::get_id()];
    }
};

