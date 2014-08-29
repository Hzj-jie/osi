
#pragma once
#include <unordered_map>
#include <thread>
#include <mutex>
#include "../sync/lock.hpp"

template <typename T>
class thread_local_storage
{
private:
    std::unordered_map<std::thread::id, T> s;
    std::mutex mtx;

public:
    T& operator*()
    {
        scope_lock(mtx);
        return s[std::this_thread::get_id()];
    }
};

